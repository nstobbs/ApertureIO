#include "ApertureIO/VulkanCommand.hpp"

namespace Aio {

VulkanCommand::VulkanCommand(Context* context, Device* device)
{
    _pContext = context;
    _pDevice = dynamic_cast<VulkanDevice*>(device);
};


VkCommandBuffer VulkanCommand::beginSingleTimeCommandBuffer(VulkanDevice* pDevice)
{
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocaInfo{};
    allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocaInfo.commandPool = pDevice->GetGlobalCommandPool();
    allocaInfo.commandBufferCount = 1;
    allocaInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_ASSERT(vkAllocateCommandBuffers(pDevice->GetVkDevice(), &allocaInfo, &commandBuffer), VK_SUCCESS, "Create One Time CommandBuffer");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin One Time CommandBuffer");
    
    return commandBuffer;
};

void VulkanCommand::endSingleTimeCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VK_ASSERT(vkQueueSubmit(pDevice->GetGraphicVkQueue(), 1, &submitInfo, VK_NULL_HANDLE), VK_SUCCESS, "Sumbit One Time CommandBuffer");
    VK_ASSERT(vkQueueWaitIdle(pDevice->GetGraphicVkQueue()), VK_SUCCESS, "Wait on One Time CommandBuffer");

    vkFreeCommandBuffers(pDevice->GetVkDevice(), pDevice->GetGlobalCommandPool(), 1, &commandBuffer);
};

void VulkanCommand::CopyBuffer(VulkanDevice* pDevice, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t size)
{
    auto commandBuffer = beginSingleTimeCommandBuffer(pDevice);

    VkBufferCopy copyInfo{};
    copyInfo.srcOffset = 0;
    copyInfo.dstOffset = 0;
    copyInfo.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyInfo);

    endSingleTimeCommandBuffer(pDevice, commandBuffer);
};

void VulkanCommand::CopyBufferToImage(VulkanDevice* pDevice, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommandBuffer(pDevice);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommandBuffer(pDevice, commandBuffer);
};

void VulkanCommand::BeginFrame(RenderContext& renderContext)
{
    renderContext.IsPaused();

    auto currentFrame = _pContext->getCurrentFrame();
    auto fence = _pDevice->GetInFlightFence(currentFrame);
    auto device = _pDevice->GetVkDevice();
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);

    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

    _pDevice->ResetPools(currentFrame);
    auto imageAvailable = _pDevice->GetNextSemaphore(currentFrame);

    VK_ASSERT(vkAcquireNextImageKHR(device, target->GetSwapChain(), UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &_imageIndex),
                                                                                          VK_SUCCESS, "Acquire Next Image");
    
    vkResetFences(device, 1, &fence);
};

void VulkanCommand::EndFrame(RenderContext& renderContext)
{  
    auto currentFrame = _pContext->getCurrentFrame();
    auto device = _pDevice->GetVkDevice();
    auto fence = _pDevice->GetInFlightFence(currentFrame);
    auto renderFinished = _pDevice->GetCurrentSemaphore(currentFrame);
    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);
    auto graphicsQueue = _pDevice->GetGraphicVkQueue();
    auto presentQueue = _pDevice->GetPresentVkQueue();
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);
    
    /* Present Frame */
    VkSemaphore finishedSemaphores[] = {renderFinished};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = finishedSemaphores;

    VkSwapchainKHR swapChains[] = {target->GetSwapChain()}; 
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &_imageIndex;

    presentInfo.pResults = nullptr;

    VK_ASSERT(vkQueuePresentKHR(presentQueue, &presentInfo), VK_SUCCESS, "Submit Present");

    // TODO: Check the current framebuffer for any resize event. If resized, re create the swapchain 
    // also check if the swapchain it out of date

    _pContext->nextFrame();
};


void VulkanCommand::Draw(RenderContext& renderContext)
{
    auto currentFrame = _pContext->getCurrentFrame();
    auto imageAvailable = _pDevice->GetCurrentSemaphore(currentFrame);
    auto renderFinished = _pDevice->GetNextSemaphore(currentFrame);
    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);
    auto shader = static_cast<VulkanShader*>(renderContext._Shader);
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);
    auto viewport = shader->GetViewport();
    auto scissor = shader->GetScissor();

    renderContext.IsPaused();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin Command Buffer - VulkanCommand::Draw");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = target->GetRenderPass();
    renderPassInfo.framebuffer = target->GetIndexFramebuffer(_imageIndex);
    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = target->GetExtent();

    //TODO: Clear Command Should be handled in Clear() or By the FrameBuffer it self.. maybe
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkBuffer vertexBuffers[] = {dynamic_cast<VulkanBuffer*>(renderContext._VertexBuffer)->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    VkBuffer indexBuffer = dynamic_cast<VulkanBuffer*>(renderContext._IndexBuffer)->GetBuffer();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDescriptorSet set = _pDevice->GetBindlessDescriptorSet();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &set, 0, nullptr);

    /* Push Buffer and Texture Handles */
    // Add Uniform Buffers And Textures Handles to Push Constant
    // TODO: Maybe it can be RenderContext job to have this sorted out...
    HandlesPushConstant handlesToPush{};
    handlesToPush.bufferHandles[0] = (BufferHandle)renderContext._UniformBuffers[0]->GetBufferHandle(); // TODO: fix this, testing
    handlesToPush.textureHandles[0] = (TextureHandle)renderContext._Textures[0]->GetTextureHandle(); // TODO: fix this, testing
    vkCmdPushConstants(commandBuffer, shader->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, sizeof(HandlesPushConstant), &handlesToPush);

    /* Draw Command */
    vkCmdDrawIndexed(commandBuffer, renderContext._IndexBuffer->Count(), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer");

    /* Sumbit Draw Command Buffer */
    VkSemaphore waitSemaphores[] = {imageAvailable};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores  = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore finishedSemaphores[] = {renderFinished};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = finishedSemaphores;
    VK_ASSERT(vkQueueSubmit(_pDevice->GetGraphicVkQueue(), 1, &submitInfo, _pDevice->GetInFlightFence(currentFrame)), VK_SUCCESS, "Submit CommandBuffer");

    /* Index to Next Command Buffer */
    _pDevice->GetNextCommandBuffer(currentFrame);
};

}; // End of Aio 