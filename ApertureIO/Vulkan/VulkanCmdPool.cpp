#include "ApertureIO/VulkanCmdPool.hpp"
#include "ApertureIO/VulkanDevice.hpp"

namespace Aio
{

VulkanCmdPool::VulkanCmdPool(VulkanDevice* device)
{
    /* Create VkCommandPool */
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    
    auto bootStrapDevice = device->GetVkBootStrapDevice();
    commandPoolInfo.queueFamilyIndex = bootStrapDevice.get_queue_index(vkb::QueueType::graphics).value();
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_ASSERT(vkCreateCommandPool(device->GetVkDevice(), &commandPoolInfo, nullptr, &_pool), VK_SUCCESS, "Create Graphics Command Pool");

    /* Allocate Command Buffers */
    _commandBuffers.resize(MAX_CMD_BUFFERS);
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = _pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = MAX_CMD_BUFFERS;
    VK_ASSERT(vkAllocateCommandBuffers(device->GetVkDevice(), &allocateInfo, _commandBuffers.data()), VK_SUCCESS, "Create Command Buffers");

    _index = 0;
};

void VulkanCmdPool::ResetPool(VulkanDevice* device)
{
    VK_ASSERT(vkResetCommandPool(device->GetVkDevice(), _pool, 0), VK_SUCCESS, "Reset Command Pool");
    _index = 0;
};

VkCommandBuffer VulkanCmdPool::GetCurrentCommandBuffer()
{
    return _commandBuffers[_index];
};

VkCommandBuffer VulkanCmdPool::GetNextCommandBuffer()
{
    _index++;
    return _commandBuffers[_index];
};

};