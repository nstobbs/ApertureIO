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

    // Createing the Logical Device
    vkb::DeviceBuilder builder{_physicalDevice};
    auto builderResult = builder.build();
    if(!builderResult)
    {
        std::cout << "device build failed: " << builderResult.error().message() << "\n";
        return false;
    };

    _device = builderResult.value();
    volkLoadDevice(_device.device); // TODO remove this if you want to support more than one device.
    return true;
};

}