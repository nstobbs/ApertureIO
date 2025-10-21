#pragma once

#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/VulkanCmdPool.hpp"
#include "ApertureIO/VulkanSemaphorePool.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Command.hpp"

const uint32_t UNIFORM_BINDING_POINT = 0;
const uint32_t STORAGE_BINDING_POINT = 1;
const uint32_t TEXTURE_BINDING_POINT = 2;
const uint32_t STORAGE_IMAGE_BINDING_POINT = 3;

namespace Aio {

class VulkanContext;
class VulkanSemaphorePool;
class VulkanTimeline;

class VulkanDevice : public Device
{
public:
    VulkanDevice(Context* pContext);
    bool init() override;

    void SetVkSurfaceKHR(VkSurfaceKHR surface);
    
    void ResetPools(uint32_t currentFrame);

    VkDevice GetVkDevice();
    VkPhysicalDevice GetVkPhysicalDevice();
    vkb::Device GetVkBootStrapDevice();

    VkSampler GetGlobalVkSampler();
    VmaAllocator GetVmaAllocator();
    VkDescriptorSetLayout GetBindlessLayout();
    VkDescriptorSet GetBindlessDescriptorSet();
    VkQueue GetPresentVkQueue();
    VkQueue GetComputeVkQueue();
    VkQueue GetGraphicVkQueue();
    VkCommandPool GetGlobalCommandPool();
    VulkanTimeline* GetTimeline(uint32_t currentFrame);

    VkSemaphore GetCurrentSemaphore(uint32_t currentFrame);
    VkSemaphore GetNextSemaphore(uint32_t currentFrame);

    VkCommandBuffer GetCurrentCommandBuffer(uint32_t currentFrame);
    VkCommandBuffer GetNextCommandBuffer(uint32_t currentFrame);

    VkFence GetInFlightFence(uint32_t currentFrame);
    

    friend class VulkanFrameBuffer; // TODO: remove this

private:
    VulkanContext* _pVulkanContext;

    vkb::PhysicalDevice _physicalDevice;
    vkb::Device _device;
    VmaAllocator _allocator;

    //TODO: This shouldn't be part of the VulkanDevice 
    VkSurfaceKHR _surface = {VK_NULL_HANDLE};
    char* _windowExtensions = {nullptr};

    VkDescriptorPool _descriptorPool;

    /* Device Global Bindless DescriptorSet */
    void createAndAllocateBindlessResources();

    VkDescriptorSet _bindlessDescriptorSet;
    VkDescriptorSetLayout _bindlessLayout;
    
    /* CommandBuffers */
    std::vector<UniquePtr<VulkanCmdPool>> _cmdPools;
    VkCommandPool _globalCommandPool;

    /* Sync Objects*/
    std::vector<VkFence> _fences;
    std::vector<UniquePtr<VulkanSemaphorePool>> _semaphorePools;
    std::vector<UniquePtr<VulkanTimeline>> _timelines;

    /* Global Samplers For Texture Reading */
    //TODO: Create an SamplersManager
    void createGlobalTextureSampler();
    VkSampler _sampler;
};

}; // End namespace Aio