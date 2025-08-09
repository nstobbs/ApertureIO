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

};