#pragma once
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include <vector>

const uint32_t MAX_SEMAPHORES = 16;
/**
 * @class VulkanSemaphorePool
 * @brief A pool of semaphores that can be used for syncing cross queue types task on a VulkanDevice.
 */

namespace Aio
{

class VulkanSemaphorePool
{
public:
    VulkanSemaphorePool(VulkanDevice* pDevice);
    VkSemaphore GetCurrentSemaphore();
    VkSemaphore GetNextSemaphore();
    void ResetPool();

private:
    uint32_t _index;
    std::vector<VkSemaphore> _semaphores;
};

};