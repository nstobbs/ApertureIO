#ifndef H_VULKANDEVICE
#define H_VULKANDEVICE

#include "VulkanCommon.hpp"
#include "VulkanContext.hpp"
#include "../Base/Device.hpp"
#include "../Base/Context.hpp"
#include "../Base/Command.hpp"

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

} // End namespace Aio
#endif