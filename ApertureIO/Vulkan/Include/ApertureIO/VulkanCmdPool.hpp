#pragma once
#include "ApertureIO/VulkanCommon.hpp"

#include <vector>

const uint32_t MAX_CMD_BUFFERS = 16;

//TODO add for enum for queue type. For now only Graphics Queue 

namespace Aio
{

class VulkanDevice; // Forward Declaration 

/**
 * @class VulkanCmdPool
 * @brief A pool of command buffers that can be used to request a command buffer.
 * The pool can then be reset to each frame.
 */
class VulkanCmdPool
{
public:
    VulkanCmdPool(WeakPtr<VulkanDevice> pDevice);
    void ResetPool(WeakPtr<VulkanDevice> pDevice);
    VkCommandBuffer GetCurrentCommandBuffer();
    VkCommandBuffer GetNextCommandBuffer();

private:
    std::vector<VkCommandBuffer> _commandBuffers;
    VkCommandPool _pool;
    uint32_t _index;
};
};