#define VMA_IMPLEMENTATION

#include <array>

#include "VulkanDevice.hpp"
namespace ApertureIO {

VulkanDevice::VulkanDevice(Context* context)
{
    _pVulkanContext = dynamic_cast<VulkanContext*>(context);
};

bool VulkanDevice::init()
{   
    GLFWwindow* window = _pVulkanContext->getActiveWindowPtr()->getWindowPtr();
    VkInstance instance = _pVulkanContext->getVkInstance();
    VkSurfaceKHR surface;

    //TODO shouldn't be using platform spec code instead of here.
    auto result = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (result != VK_SUCCESS)
    {
        std::cout << "failed to create VkSurface :(\n";
        return false;
    }

    // Picking the Physical Device
    vkb::PhysicalDeviceSelector phyiscalDevicePicker(_pVulkanContext->_instance);
    phyiscalDevicePicker.set_surface(surface); // this doesnt need to be set if we want an headless option.

    /* Set the Required Extensions that I need */
    // Need to double check these. I'm sure I'm doing something
    // wrong here or volk is doing it for me.


    for (auto ext : _pVulkanContext->getRequiredExtensions())
    {
        /* TODO setting required extensions seems to break 
        the physical device selection process.*/
        //phyiscalDevicePicker.add_required_extension(ext);
        //std::cout << "Added Extension " << ext << "\n";
    }

    auto pickerResult = phyiscalDevicePicker.select();
    if (!pickerResult) 
    {
        std::cout << "failed to pick physical device: " << pickerResult.error().message() << "\n";
        return false;
    }

    _physicalDevice = pickerResult.value();

    // Enable Device Features that we want.
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

    /* Create Command Pool */
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = _device.get_queue_index(vkb::QueueType::graphics).value();
    VK_ASSERT(vkCreateCommandPool(GetVkDevice(), &commandPoolInfo, nullptr, &_commandPool), VK_SUCCESS, "Create Graphics Command Pool");

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
    
    createBindlessLayout();
    return true;
};

/* From https://dev.to/gasim/implementing-bindless-design-in-vulkan-34no */
void VulkanDevice::createBindlessLayout()
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
    
};

/* Getter Functions*/

VkDevice VulkanDevice::GetVkDevice()
{
    return _device.device;
};

VmaAllocator VulkanDevice::GetVmaAllocator()
{
    return _allocator;
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

}