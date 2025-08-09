#define VMA_IMPLEMENTATION

#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanSemaphorePool.hpp"

#include <array>
#include <string>

namespace Aio {

VulkanDevice::VulkanDevice(Context* context)
{
    _pVulkanContext = dynamic_cast<VulkanContext*>(context);
};

bool VulkanDevice::init()
{   
    VkInstance instance = _pVulkanContext->GetVkInstance(); 

    // Picking the Physical Device
    vkb::PhysicalDeviceSelector phyiscalDevicePicker(_pVulkanContext->_instance);
    phyiscalDevicePicker.set_surface(_surface); // this doesnt need to be set if we want an headless option.

    /* Set the Required Extensions that I need */
    // Need to double check these. I'm sure I'm doing something
    // wrong here or volk is doing it for me.

    //TODO: Any Extensions needed for the Window / Surface should happen here...
    // else it should be remove as a argv 

    auto pickerResult = phyiscalDevicePicker.select();
    if (!pickerResult) 
    {
        std::cout << "failed to pick physical device: " << pickerResult.error().message() << "\n";
        return false;
    }

    _physicalDevice = pickerResult.value();

    // Enable Device Features 
    VkPhysicalDeviceVulkan12Features features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features.runtimeDescriptorArray = VK_TRUE;

    //features.descriptorBindingVariableDescriptorCount = VK_TRUE; // TODO: dont I really need this??
    features.descriptorBindingPartiallyBound = VK_TRUE;
    features.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;

    features.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    features.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;

    // Createing the Logical Device
    vkb::DeviceBuilder builder{_physicalDevice};
    builder.add_pNext(&features);
    auto builderResult = builder.build();

    if(!builderResult)
    {
        std::cout << "device build failed: " << builderResult.error().message() << "\n";
        return false;
    };

    _device = builderResult.value();
    volkLoadDevice(GetVkDevice()); // TODO remove this if you want to support more than one device.

    // Init VMA for memory allocation
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    allocatorCreateInfo.physicalDevice = _device.physical_device;
    allocatorCreateInfo.device = _device.device;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    //allocatorCreateInfo.flags =  
    VmaVulkanFunctions vkFunctions;
    vmaImportVulkanFunctionsFromVolk(&allocatorCreateInfo, &vkFunctions);
    allocatorCreateInfo.pVulkanFunctions = &vkFunctions;

    vmaCreateAllocator(&allocatorCreateInfo, &_allocator);

    /* Create Descriptor Pool */
    //TODO Use PhysicalDevice to find limits and set them here.
    uint32_t maxLimit = 1024;
    std::vector<VkDescriptorPoolSize> poolInfos;
    
    VkDescriptorPoolSize storageBufferPoolInfo{};
    storageBufferPoolInfo.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferPoolInfo.descriptorCount = maxLimit;
    poolInfos.push_back(storageBufferPoolInfo);

    VkDescriptorPoolSize imagesPoolInfo{};
    imagesPoolInfo.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imagesPoolInfo.descriptorCount = maxLimit;
    poolInfos.push_back(imagesPoolInfo);

    VkDescriptorPoolSize uniformBufferPoolInfo{};
    uniformBufferPoolInfo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferPoolInfo.descriptorCount = maxLimit;
    poolInfos.push_back(uniformBufferPoolInfo);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolInfos.size());
    descriptorPoolCreateInfo.pPoolSizes = poolInfos.data();
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    descriptorPoolCreateInfo.maxSets = maxLimit; // TODO: This is deffo too high, think we only need one set really
    VK_ASSERT(vkCreateDescriptorPool(GetVkDevice(), &descriptorPoolCreateInfo, nullptr,&_descriptorPool), VK_SUCCESS, "Create Descriptor Pool");
    
    createAndAllocateBindlessResources();
    createGlobalTextureSampler();

    // TODO: should sync objects be part of the device?
    // create sync objects
    uint32_t inFlight = _pVulkanContext->getMaxFramesInFlight();

    /* Fences Per Frame In Flight */
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    _fences.resize(inFlight);

    for (int i = 0; i < inFlight; i++)
    {
        VK_ASSERT(vkCreateFence(GetVkDevice(), &fenceCreateInfo, nullptr, &_fences[i]), VK_SUCCESS, "Create Fences");
    };

    /* Create an VulkanSemaphorePool for each Frame in Flight */
    for (int i = 0; i < inFlight; i++)
    {
        auto syncPool = VulkanSemaphorePool(this);
        _semaphorePools.push_back(syncPool);
    };

    /* Create an VulkanCmdPool for each Frame in Flight */
    for (int i = 0; i < inFlight; i++)
    {
        auto cmdPool = VulkanCmdPool(this);
        _cmdPools.push_back(cmdPool);
    };

    /* Command Pool for Creating One Time Use Command Buffers*/
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = _device.get_queue_index(vkb::QueueType::graphics).value();
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_ASSERT(vkCreateCommandPool(GetVkDevice(), &commandPoolInfo, nullptr, &_globalCommandPool), VK_SUCCESS, "Create Global Command Pool");

    return true;
};

/* From https://dev.to/gasim/implementing-bindless-design-in-vulkan-34no */
void VulkanDevice::createAndAllocateBindlessResources()
{
    std::array<VkDescriptorSetLayoutBinding, 3> bindings{};
    std::array<VkDescriptorBindingFlags, 3> flags{};
    std::array<VkDescriptorType, 3> types{
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    };

    for (int i = 0; i < 3; ++i)
    {
        bindings.at(i).binding = i;
        bindings.at(i).descriptorType = types.at(i);
        bindings.at(i).descriptorCount = 1024; // TODO comeback and set limits better
        bindings.at(i).stageFlags = VK_SHADER_STAGE_ALL;
        flags.at(i) = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                      VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
                      //VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
    bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlags.pBindingFlags = flags.data();
    bindingFlags.bindingCount = 3;
    bindingFlags.pNext = nullptr;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 3;
    createInfo.pBindings = bindings.data();
    createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    createInfo.pNext = &bindingFlags;

    VK_ASSERT(vkCreateDescriptorSetLayout(GetVkDevice(), &createInfo, nullptr, &_bindlessLayout), VK_SUCCESS, "Create Bindless Layout");

    /* Creating the Descriptor Set*/
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = _descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &_bindlessLayout;

    VK_ASSERT(vkAllocateDescriptorSets(GetVkDevice(), &allocateInfo, &_bindlessDescriptorSet), VK_SUCCESS, "Allocate Bindless Descriptor Set");
};

void VulkanDevice::createGlobalTextureSampler()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice.physical_device, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE; // TODO: enable this on the device first...
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_ASSERT(vkCreateSampler(GetVkDevice(), &samplerInfo, nullptr, &_sampler), VK_SUCCESS, "VulkanDevice: Failed to create VkSampler...");
};

void VulkanDevice::ResetPools(uint32_t currentFrame)
{
    _cmdPools[currentFrame].ResetPool(this);
    _semaphorePools[currentFrame].ResetPool();
};

/* Setter Functions*/
void VulkanDevice::SetVkSurfaceKHR(VkSurfaceKHR surface)
{
    _surface = surface;
}
/* Getter Functions*/

VkDevice VulkanDevice::GetVkDevice()
{
    return _device.device;
};

vkb::Device VulkanDevice::GetVkBootStrapDevice()
{
    return _device;
};

VkSampler VulkanDevice::GetGlobalVkSampler()
{
    return _sampler;
};

VmaAllocator VulkanDevice::GetVmaAllocator()
{
    return _allocator;
};

VkDescriptorSetLayout VulkanDevice::GetBindlessLayout()
{
    return _bindlessLayout;
};

VkDescriptorSet VulkanDevice::GetBindlessDescriptorSet()
{
    return _bindlessDescriptorSet;
};

VkQueue VulkanDevice::GetPresentVkQueue()
{
    auto result = _device.get_queue(vkb::QueueType::present);
    if (!result)
    {
        throw std::runtime_error("fail to get present queue.");
    }
    return result.value();
};

VkQueue VulkanDevice::GetComputeVkQueue()
{
    auto result = _device.get_queue(vkb::QueueType::compute);
    if (!result)
    {
        throw std::runtime_error("fail to get compute queue.");
    }
    return result.value();
};

VkQueue VulkanDevice::GetGraphicVkQueue()
{
    auto result = _device.get_queue(vkb::QueueType::graphics);
    if (!result)
    {
        throw std::runtime_error("fail to get graphics queue.");
    }
    return result.value();
};

VkSemaphore VulkanDevice::GetCurrentSemaphore(uint32_t currentFrame)
{
    return _semaphorePools[currentFrame].GetCurrentSemaphore();
};

VkSemaphore VulkanDevice::GetNextSemaphore(uint32_t currentFrame)
{
    
    return _semaphorePools[currentFrame].GetNextSemaphore();
};

VkFence VulkanDevice::GetInFlightFence(uint32_t currentFrame)
{
    return _fences[currentFrame];
};

VkCommandBuffer VulkanDevice::GetCurrentCommandBuffer(uint32_t currentFrame)
{
    return _cmdPools[currentFrame].GetCurrentCommandBuffer();
};

VkCommandBuffer VulkanDevice::GetNextCommandBuffer(uint32_t currentFrame)
{
    return _cmdPools[currentFrame].GetNextCommandBuffer();
}

VkCommandPool VulkanDevice::GetGlobalCommandPool()
{
    return _globalCommandPool;
};


}