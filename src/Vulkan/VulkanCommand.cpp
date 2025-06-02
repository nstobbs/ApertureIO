#include "VulkanCommand.hpp"

namespace Aio {

VulkanCommand::VulkanCommand(Context* context, Device* device)
{
    _pContext = context;
    _pDevice = dynamic_cast<VulkanDevice*>(device);
};

void VulkanCommand::createCommandBuffers(VulkanDevice* pDevice)
{
    // Create the Required Commands Buffers Need for Each Frame in Flight.
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t count = _pContext->getMaxFramesInFlight();
    commandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocaInfo{};
    allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocaInfo.commandPool = pDevice->_commandPool;
    allocaInfo.commandBufferCount = count;
    allocaInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_ASSERT(vkAllocateCommandBuffers(pDevice->GetVkDevice(), &allocaInfo, commandBuffers.data()), VK_SUCCESS, "Create Command Buffers");

    // TODO: Come back and handle the command buffers better.
    for(auto commandBuffer : commandBuffers)
    {
        pDevice->_commandBuffers.push_back(commandBuffer);
    };
};

VkCommandBuffer VulkanCommand::beginSingleTimeCommandBuffer(VulkanDevice* pDevice)
{
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocaInfo{};
    allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocaInfo.commandPool = pDevice->_commandPool;
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

    vkFreeCommandBuffers(pDevice->GetVkDevice(), pDevice->_commandPool, 1, &commandBuffer);
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

void VulkanCommand::StartCommand(RenderContext& renderContext)
{
    //auto device = dynamic_cast<VulkanDevice*>(pDevice); 
    //createCommandBuffers(device);
};

void VulkanCommand::Draw(RenderContext& renderContext)
{   
    if (!renderContext.IsPaused())
    {
        //TODO: Break this function down to more smaller ones
        uint32_t currentFrame = _pContext->getCurrentFrame();
        VkDevice device = _pDevice->GetVkDevice();
        VkCommandBuffer commandBuffer = _pDevice->GetCommandBuffer(currentFrame);
        VkFence inFlightFence = _pDevice->GetInFlightFence(currentFrame);
        VulkanFrameBuffer* target = dynamic_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);
        VulkanShader* shader = dynamic_cast<VulkanShader*>(renderContext._Shader);
        
        
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VK_ASSERT(vkAcquireNextImageKHR(device, target->GetSwapChain(), UINT64_MAX, _pDevice->GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex),
                                                                                                                                                    VK_SUCCESS, "Acquire Next Image");
        
        vkResetFences(device, 1, &inFlightFence);
        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin Command Buffer - VulkanCommand::Draw");

        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = target->GetRenderPass();
        renderPassInfo.framebuffer = target->GetIndexFramebuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = target->GetExtent();

        //TODO: Clear Command Shouild be handled in Clear();
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
        vkCmdSetViewport(commandBuffer, 0, 1, &shader->GetViewport());
        vkCmdSetScissor(commandBuffer, 0, 1, &shader->GetScissor());

        VkDescriptorSet set = _pDevice->GetBindlessDescriptorSet();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &set, 0, nullptr);

        // DRAW COMMAND!
        vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer");

        // Sumbit and Preset

        VkSemaphore waitSemaphores[] = {_pDevice->GetImageAvailableSemaphore(currentFrame)};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores  = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore finshedSemaphores[] = {_pDevice->GetRenderFinshedSemaphore(currentFrame)};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = finshedSemaphores;
        VK_ASSERT(vkQueueSubmit(_pDevice->GetGraphicVkQueue(), 1, &submitInfo, _pDevice->GetInFlightFence(currentFrame)), VK_SUCCESS, "Submit CommandBuffer");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = finshedSemaphores;

        VkSwapchainKHR swapChains[] = {target->GetSwapChain()}; 
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = nullptr;

        VK_ASSERT(vkQueuePresentKHR(_pDevice->GetPresentVkQueue(), &presentInfo), VK_SUCCESS, "Submit Present");
    };
};

}; // End of Aio 