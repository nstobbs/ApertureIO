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

void VulkanCommand::submitCommandBuffer(VulkanDevice* pDevice, VulkanTimeline* timeline, VkCommandBuffer commandBuffer)
{
    /* Sumbit Draw Command Buffer */
    VkCommandBufferSubmitInfo commandSumbitInfo{};
    commandSumbitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandSumbitInfo.commandBuffer = commandBuffer;
    commandSumbitInfo.deviceMask = 0x1;

    VkSemaphoreSubmitInfo waitOn[] = {timeline->CreateWaitInfo()};
    VkSemaphoreSubmitInfo signalOn[] = {timeline->CreateSignalInfo()};

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandSumbitInfo;

    // Wait On
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = waitOn;
    
    // Signal to
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = signalOn;

    VK_ASSERT(vkQueueSubmit2(pDevice->GetGraphicVkQueue(), 1, &submitInfo, VK_NULL_HANDLE), VK_SUCCESS, "Submit CommandBuffer");
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
    auto imageReady = _pDevice->GetCurrentSemaphore(currentFrame);
    auto timeline = _pDevice->GetTimeline(currentFrame);
    auto graphicQueue = _pDevice->GetGraphicVkQueue();
    auto device = _pDevice->GetVkDevice();
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);

    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    VK_ASSERT(vkAcquireNextImageKHR(device, target->GetSwapChain(), UINT64_MAX, imageReady, VK_NULL_HANDLE, &_imageIndex),
                                                                                          VK_SUCCESS, "Acquire Next Image");
    _pDevice->GetTimeline(currentFrame)->SetState(TimelineState::ACQUIRE_IMAGE_STAGE);
    vkResetFences(device, 1, &fence);
    _pDevice->ResetPools(currentFrame);

    /* Bridge To Timeline Semaphores */
    VkSemaphoreSubmitInfo waitOn{};
    waitOn.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitOn.stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    waitOn.semaphore = imageReady;
    waitOn.deviceIndex = 0;

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.commandBufferInfoCount = 0;

    // Wait On
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitOn;

    // Signal On
    VkSemaphoreSubmitInfo signalOn[] = {timeline->CreateSignalInfo()};
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = signalOn;

    VK_ASSERT(vkQueueSubmit2(graphicQueue, 1, &submitInfo, VK_NULL_HANDLE), VK_SUCCESS, "Bridge Semaphore Submit.");

    /* Index To Next Semaphore */
    _pDevice->GetNextSemaphore(currentFrame);
};

void VulkanCommand::CopyFrameBufferToPresent(RenderContext& from, RenderContext& to, std::string fromLayer)
{
    auto currentFrame = _pContext->getCurrentFrame();
    auto fromImagePtr = dynamic_cast<VulkanFrameBuffer*>(from._TargetFrameBuffer)->GetLayerVulkanImage(fromLayer);
    auto toImagePtr = dynamic_cast<VulkanFrameBuffer*>(to._TargetFrameBuffer)->GetLayerVulkanImage("Viewer");
    
    fromImagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_GENERAL, true);
    toImagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_GENERAL, true);

    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);
    auto timeline = _pDevice->GetTimeline(currentFrame);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin Command Buffer - VulkanCommand::CopyFrameBufferToPresent");

    VkImage fromImage = fromImagePtr->GetImage(currentFrame);
    VkImage toImage = toImagePtr->GetImage(currentFrame);

    uint32_t height = to._TargetFrameBuffer->GetHeight();
    uint32_t width = to._TargetFrameBuffer->GetWidth();
    
    /* Shuffle Channels For Present Pixel Format 
    RGBA -> BGRA
    */
    
    VkImageBlit region{};
    region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.srcOffsets[0] = {0, 0, 0};
    region.srcOffsets[1] = {int(width), int(height), 1};
    region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.dstOffsets[0] = {0, 0, 0};
    region.dstOffsets[1] = {int(width), int(height), 1};

    vkCmdBlitImage(commandBuffer, fromImage, VK_IMAGE_LAYOUT_GENERAL,
                                  toImage, VK_IMAGE_LAYOUT_GENERAL,
                                  1, &region, VK_FILTER_NEAREST);

    /* Sumbit Command Buffer */
    VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer - VulkanCommand::CopyFrameBufferToPresent");
    
    submitCommandBuffer(_pDevice, timeline, commandBuffer);

    /* Index to Next Command Buffer */
    _pDevice->GetNextCommandBuffer(currentFrame);
};

void VulkanCommand::EndFrame(RenderContext& copyFrameBuffer, RenderContext& renderContext)
{  
    auto currentFrame = _pContext->getCurrentFrame();
    auto device = _pDevice->GetVkDevice();
    auto timeline = _pDevice->GetTimeline(currentFrame);
    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);
    auto finishedFrame  = _pDevice->GetCurrentSemaphore(currentFrame);
    auto fence = _pDevice->GetInFlightFence(currentFrame);
    auto graphicsQueue = _pDevice->GetGraphicVkQueue();
    auto presentQueue = _pDevice->GetPresentVkQueue();
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);

    auto layers = copyFrameBuffer._TargetFrameBuffer->GetLayerNames();

    _pDevice->GetTimeline(currentFrame)->SetState(TimelineState::PRESENT_STAGE);

    CopyFrameBufferToPresent(copyFrameBuffer, renderContext, layers[0]);

    /* Change the Layout for Present Image Layout */
    auto imagePtr = target->GetLayerVulkanImage("Viewer");
    imagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, true);

    /* Bridging From Timeline Semaphore To Binary Semaphore*/
    VkSemaphoreSubmitInfo waitOn[] = {timeline->CreateWaitInfo()};
    VkSemaphoreSubmitInfo signal{};
    signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    signal.semaphore = finishedFrame;
    signal.deviceIndex = 0;

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.commandBufferInfoCount = 0;

    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = waitOn;

    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signal;

    VK_ASSERT(vkQueueSubmit2(_pDevice->GetGraphicVkQueue(), 1, &submitInfo, fence), VK_SUCCESS, "Submit CommandBuffer");

    /* Present Frame */
    VkSemaphore presentSemaphore[] = {finishedFrame};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = presentSemaphore;

    VkSwapchainKHR swapChains[] = {target->GetSwapChain()}; 
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &_imageIndex;

    presentInfo.pResults = nullptr;
    VK_ASSERT(vkQueuePresentKHR(presentQueue, &presentInfo), VK_SUCCESS, "Submit Present");

    /* Index to the Next Frame and Semaphore */
    _pDevice->GetNextSemaphore(currentFrame);
    _pContext->nextFrame();
};


void VulkanCommand::Draw(RenderContext& renderContext)
{
    auto currentFrame = _pContext->getCurrentFrame();
    auto shader = static_cast<VulkanShader*>(renderContext._Shader);
    auto target = static_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);
    auto viewport = shader->GetViewport();
    auto scissor = shader->GetScissor();

    renderContext.IsPaused();
    _pDevice->GetTimeline(currentFrame)->SetState(TimelineState::GRAPHIC_STAGE);

    /* Change Required Image Layouts */
    auto layers = target->GetLayerNames();
    for (auto layer: layers)
    {
        auto imagePtr = target->GetLayerVulkanImage(layer); 
        switch(target->GetLayerPixelFormat(layer))
        {
            case FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat:
                imagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, true);
                break;
            case FrameBufferPixelFormat::COLOR_RGBA_8888:
                imagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, true);
                break;
            case FrameBufferPixelFormat::DEPTH_D32_S8:
                imagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, true);
                break;
        };
    };

    /* Note: It's not really safe to grab the CurrentCommandBuffer and
    CurrentSemaphores till your absolutely ready to record/use them. */
    auto timeline = _pDevice->GetTimeline(currentFrame);
    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);

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

    bindCommonToCommandBuffer(commandBuffer, shader, generatePushConstant(timeline, renderContext));

    /* Draw Command */
    vkCmdDrawIndexed(commandBuffer, renderContext._IndexBuffer->Count(), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer - VulkanCommand::Draw");
    submitCommandBuffer(_pDevice, timeline, commandBuffer);

    /* Index to Next Command Buffer */
    _pDevice->GetNextCommandBuffer(currentFrame);
};

void VulkanCommand::DispatchCompute(RenderContext& renderContext, uint32_t X, uint32_t Y, uint32_t Z)
{

    auto currentFrame = _pContext->getCurrentFrame();
    auto shader = static_cast<VulkanShader*>(renderContext._Shader);
    auto timeline = _pDevice->GetTimeline(currentFrame);

    renderContext.IsPaused();
    _pDevice->GetTimeline(currentFrame)->SetState(TimelineState::COMPUTE_STAGE);

    /* Change Required Image Layouts */
    std::vector<FrameBuffer*> frameBuffers = renderContext._FrameBuffers;
    frameBuffers.push_back(renderContext._TargetFrameBuffer);
    for (auto frameBuffer : frameBuffers)
    {
        auto layers = frameBuffer->GetLayerNames();
        auto vulkanFrameBuffer = dynamic_cast<VulkanFrameBuffer*>(frameBuffer);
        for (auto layer : layers)
        {
            auto imagePtr = vulkanFrameBuffer->GetLayerVulkanImage(layer);
            imagePtr->SetImageLayout(currentFrame, VK_IMAGE_LAYOUT_GENERAL, true);
        };
    };

    auto commandBuffer = _pDevice->GetCurrentCommandBuffer(currentFrame);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin Command Buffer - VulkanCommand::Dispatch");

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, shader->GetPipeline());
    bindCommonToCommandBuffer(commandBuffer, shader, generatePushConstant(timeline, renderContext));
    vkCmdDispatch(commandBuffer, X, Y, Z);

    VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer - VulkanCommand::Dispatch");
    submitCommandBuffer(_pDevice, timeline, commandBuffer);
    /* Index to Next Command Buffer */
    _pDevice->GetNextCommandBuffer(currentFrame);
};

/* TODO: This Function seems Pretty heavy and we should maybe have this use the renderContext hash
to cache the handles array. Instead of create a new array each frame. */
HandlesPushConstant VulkanCommand::generatePushConstant(VulkanTimeline* timeline ,RenderContext& renderContext)
{

    bool oneTimePrint = (debugCounter < 3) ? true : false; //FIXME: Temp: print the handles order on the first frame.
    debugCounter++;

    /* Order Of Handles is bases of the Order placed within Each Group.
    At the time of binding to a renderContext.
    Group Order = Uniforms -> Storages -> Textures -> FrameBuffers
    The Target FrameBuffer will the the Last Handles in the Array.
     */

    /* smoking hot trash way of printing this.*/
    if (oneTimePrint)
    {
        auto state = timeline->GetState();
        switch(state)
        {
            case TimelineState::COMPUTE_STAGE:
                Logger::LogInfo("Handles For Compute Stage");
                break;
            case TimelineState::GRAPHIC_STAGE:
                Logger::LogInfo("Handles For Graphic Stage");
                break;
        }
    };

    HandlesPushConstant handlesToPush{};
    uint32_t handlesCount = 0; // Error if we got pash 64 handles.

    for(auto buffer : renderContext._UniformBuffers)
    {
        handlesToPush.handles[handlesCount] = static_cast<uint32_t>(buffer->GetBufferHandle());
        if (oneTimePrint)
        {
            auto info = "UniformBuffer at Handle Index: " + std::to_string(handlesCount);
            Logger::LogInfo(info);
        };
        handlesCount++;
    };

    for(auto buffer : renderContext._StorageBuffers)
    {
        handlesToPush.handles[handlesCount] = static_cast<uint32_t>(buffer->GetBufferHandle());
        if (oneTimePrint)
        {
            auto info = "StorageBuffer at Handle Index: " + std::to_string(handlesCount);
            Logger::LogInfo(info);
        };
        handlesCount++;
    };

    for(auto texture : renderContext._Textures)
    {
        handlesToPush.handles[handlesCount] = static_cast<uint32_t>(texture->GetTextureHandle());
        if (oneTimePrint)
        {
            auto info = "Texture at Handle Index: " + std::to_string(handlesCount);
            Logger::LogInfo(info);
        };
        handlesCount++;
    };

    auto currentFrame = _pContext->getCurrentFrame();

    for(auto frameBuffer : renderContext._FrameBuffers)
    {
        auto layers = frameBuffer->GetLayerNames();
        auto vulkanFrameBuffer = dynamic_cast<VulkanFrameBuffer*>(frameBuffer);

        for (auto layer : layers)
        {
            auto imagePtr = vulkanFrameBuffer->GetLayerVulkanImage(layer);
            handlesToPush.handles[handlesCount] = static_cast<uint32_t>(imagePtr->GetStorageImageHandle(currentFrame));
            if (oneTimePrint)
            {
                auto info = "FrameBuffer Layer: " + layer + " at Handle Index: " + std::to_string(handlesCount);
                Logger::LogInfo(info);
            };
            handlesCount++;
        };
    };

    /* If it's a Compute Shader then we throw in the TargetFrameBuffer as the last handle */
    if (renderContext._Shader->GetShaderType() == ShaderType::Compute)
    {
        auto layers = renderContext._TargetFrameBuffer->GetLayerNames();
        auto vulkanFrameBuffer = dynamic_cast<VulkanFrameBuffer*>(renderContext._TargetFrameBuffer);

        for (auto layer : layers)
        {
            auto imagePtr = vulkanFrameBuffer->GetLayerVulkanImage(layer);
            handlesToPush.handles[handlesCount] = static_cast<uint32_t>(imagePtr->GetStorageImageHandle(currentFrame));
            if (oneTimePrint)
            {
                auto info = "Target FrameBuffer Layer: " + layer + " at Handle Index: " + std::to_string(handlesCount);
                Logger::LogInfo(info);
            };
            handlesCount++;
        };  
    };

    return handlesToPush;
};

void VulkanCommand::bindCommonToCommandBuffer(VkCommandBuffer command, VulkanShader* shader, HandlesPushConstant handles)
{
    VkDescriptorSet set = _pDevice->GetBindlessDescriptorSet();
    auto bindPoint = (shader->GetShaderType() == ShaderType::Graphics) ?  VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;
    auto pipelineLayout = shader->GetPipelineLayout();
    vkCmdBindDescriptorSets(command, bindPoint, pipelineLayout, 0, 1, &set, 0, nullptr);
    vkCmdPushConstants(command, pipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(HandlesPushConstant), &handles);
};

}; // End of Aio 