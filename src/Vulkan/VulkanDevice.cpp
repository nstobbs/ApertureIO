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
    }

    vkb::PhysicalDeviceSelector phyiscalDevicePicker(_pVulkanContext->_instance, surface);

    /* Set the Required Extensions that I need */
    for (auto ext : _pVulkanContext->getRequiredExtensions())
    {
        phyiscalDevicePicker.add_required_extension(ext);
        std::cout << "Added Extension " << ext << "\n";
    }

    auto deviceResult = phyiscalDevicePicker.select();
    if (!deviceResult) // TODO this seems wrong but fuck it
    {
        std::cout << "couldn't find a vulkan gpu to use :(\n";
    }
    return true;
};

}