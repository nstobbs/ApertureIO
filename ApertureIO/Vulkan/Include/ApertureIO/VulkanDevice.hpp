#pragma once

#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Command.hpp"

namespace Aio {

class VulkanContext;

class VulkanDevice : public Device
{
    public:
    VulkanDevice(Context* context);
    bool init() override;

    VkDevice GetVkDevice();
    VmaAllocator GetVmaAllocator();
    VkDescriptorSetLayout GetBindlessLayout();
    VkDescriptorSet GetBindlessDescriptorSet();
    VkQueue GetPresentVkQueue();
    VkQueue GetComputeVkQueue();
    VkQueue GetGraphicVkQueue();

    VkSemaphore GetImageAvailableSemaphore(uint32_t currentFrame);
    VkSemaphore GetRenderFinshedSemaphore(uint32_t currentFrame);
    VkFence GetInFlightFence(uint32_t currentFrame);
    VkCommandBuffer GetCommandBuffer(uint32_t currentFrame);
    

    friend class VulkanFrameBuffer;
    friend class VulkanCommand;

    private:

    VulkanContext* _pVulkanContext;

    vkb::PhysicalDevice _physicalDevice;
    vkb::Device _device;
    VmaAllocator _allocator;

    VkDescriptorPool _descriptorPool;

    /* Device Global Bindless DescriptorSet */
    void createAndAllocateBindlessResources();

    VkDescriptorSet _bindlessDescriptorSet;
    VkDescriptorSetLayout _bindlessLayout;
    
    /* CommandsBuffers and Sync */
    VkCommandPool _commandPool;
    std::vector<VkCommandBuffer> _commandBuffers;

    std::vector<VkFence> _fences;
    std::vector<VkSemaphore> _semaphores;
};

}; // End namespace Aio