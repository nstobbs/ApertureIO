#include "Device.hpp"
#include "../Vulkan/VulkanDevice.hpp"

namespace Aio {

 Device* Device::CreateDevice(Context* context)
 {
    switch(context->getRendererAPI())
    {
        case eVulkan:
            VulkanDevice* device = new VulkanDevice(context);
            dynamic_cast<Device*>(device)->pCommand = Command::CreateCommand(device, context);
            return device;
    }
 };

} // End Aio namespace