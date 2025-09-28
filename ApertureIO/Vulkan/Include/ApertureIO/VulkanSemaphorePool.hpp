#pragma once
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include <vector>

const uint32_t MAX_SEMAPHORES = 16;
const bool DEBUG_PRINT = false;
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

/**
 * @class VulkanTimeline
 * @brief Class for managing synchronization of different command buffers running on the host-device.
 */

enum class TimelineState
{
    UNINITIALIZED_STAGE = -1,
    ACQUIRE_IMAGE_STAGE = 0,
    GRAPHIC_STAGE = 1,
    COMPUTE_STAGE = 2,
    PRESENT_STAGE = 3
};

class VulkanTimeline
{
public:
    VulkanTimeline(VulkanDevice* pDevice);
    ~VulkanTimeline();

    VkSemaphoreSubmitInfo CreateWaitInfo(uint32_t *values, size_t size);
    VkSemaphoreSubmitInfo CreateWaitInfo();
    VkSemaphoreSubmitInfo CreateSignalInfo();

    //Debug Function
    VkSemaphore GetSemaphore();

    /* Used to syncing a group of command buffers
    that their execution order doesn't matter but instead
    executing the group in a whole and allowing parallel
    execution. */
    void StartGroup();
    void EndGroup();
    VkSemaphoreSubmitInfo CreateGroupWaitInfo();

    /* State Functions */
    TimelineState GetState();
    void SetState(TimelineState state);

private:
    TimelineState _state = {TimelineState::UNINITIALIZED_STAGE};
    uint64_t _counter = {0};
    VkSemaphore _timelineSemaphore;

    VulkanDevice* _pDevice;

    /* FIXME: This Group stuff won't be thread safe */
    bool isGroup = {false}; 
    std::vector<uint32_t> _waitPoints;
};

};