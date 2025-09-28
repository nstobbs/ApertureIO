#include "ApertureIO/VulkanSemaphorePool.hpp"

namespace Aio
{

VulkanSemaphorePool::VulkanSemaphorePool(VulkanDevice* pDevice)
{
    _index = 0;
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    _semaphores.resize(MAX_SEMAPHORES);

    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        VK_ASSERT(vkCreateSemaphore(pDevice->GetVkDevice(), &semaphoreCreateInfo, nullptr, &_semaphores[i]), VK_SUCCESS, "Create Semaphores");
    };
};

VkSemaphore VulkanSemaphorePool::GetNextSemaphore()
{
    _index++;
    return _semaphores[_index];   
};

VkSemaphore VulkanSemaphorePool::GetCurrentSemaphore()
{
    return _semaphores[_index];   
};

void VulkanSemaphorePool::ResetPool()
{
    _index = 0;
};

/* VulkanTimeline Stuff */

VulkanTimeline::VulkanTimeline(VulkanDevice* pDevice)
{
    VkSemaphoreTypeCreateInfo timelineInfo{};
    timelineInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineInfo.initialValue = _counter;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &timelineInfo;

    _pDevice = pDevice;
    VK_ASSERT(vkCreateSemaphore(_pDevice->GetVkDevice(), &createInfo, nullptr, &_timelineSemaphore), VK_SUCCESS, "VulkanTimeline::CreateSemaphore");
};

VulkanTimeline::~VulkanTimeline()
{
    vkDestroySemaphore(_pDevice->GetVkDevice(), _timelineSemaphore, nullptr);
};

VkSemaphoreSubmitInfo VulkanTimeline::CreateWaitInfo(uint32_t *values, size_t size)
{
    VkSemaphoreSubmitInfo empty{};
    return empty;
};

VkSemaphoreSubmitInfo VulkanTimeline::CreateWaitInfo()
{
    VkSemaphoreSubmitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.semaphore = _timelineSemaphore;
    waitInfo.value = _counter;
    waitInfo.stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ;
    waitInfo.deviceIndex = 0;
    auto infoMsg = "VulkanTimeline:: Waiting on counter: " + std::to_string(_counter);

    if(DEBUG_PRINT)
        Logger::LogInfo(infoMsg);
    return waitInfo;
};

VkSemaphoreSubmitInfo VulkanTimeline::CreateSignalInfo()
{
    _counter++;
    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.semaphore = _timelineSemaphore;
    signalInfo.value = _counter;
    signalInfo.stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ;
    signalInfo.deviceIndex = 0;
    auto infoMsg = "VulkanTimeline:: Will Signal on counter: " + std::to_string(_counter);

    if(DEBUG_PRINT)
        Logger::LogInfo(infoMsg);
    return signalInfo;
};

void VulkanTimeline::StartGroup()
{

};

void VulkanTimeline::EndGroup()
{

};

VkSemaphoreSubmitInfo VulkanTimeline::CreateGroupWaitInfo()
{
    VkSemaphoreSubmitInfo empty{};
    return empty;
};

VkSemaphore VulkanTimeline::GetSemaphore()
{
    return _timelineSemaphore;
};

/* State Functions */
TimelineState VulkanTimeline::GetState()
{
    return _state;
};

void VulkanTimeline::SetState(TimelineState state)
{
    _state = state;
};


};