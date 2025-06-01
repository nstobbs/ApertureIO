#include "Device.hpp"
#include "../Vulkan/VulkanDevice.hpp"

namespace Aio {

 Device* Device::CreateDevice(Context* context)
 {
    switch(context->getRendererAPI())
    {
        case eVulkan:
            VulkanDevice* device = new VulkanDevice(context);
            dynamic_cast<Device*>(device)->pCommand = Command::CreateCommand(context, dynamic_cast<Device*>(device));
            return device;
    }
 };

} // End Aio namespace