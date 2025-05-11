#include "VulkanCommand.hpp"
#include "VulkanDevice.hpp"

namespace ApertureIO {

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

void VulkanCommand::StartCommand(Device* pDevice)
{
    auto device = dynamic_cast<VulkanDevice*>(pDevice); 
    createCommandBuffers(device);
};
}; // End of ApertureIO namespace