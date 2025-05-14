#include "Device.hpp"
#include "../Vulkan/VulkanDevice.hpp"

namespace Aio {

 Device* Device::CreateDevice(Context* context)
 {
    switch(context->getRendererAPI())
    {
        case eVulkan:
            return new VulkanDevice(context);
    }
 };

} // End Aio namespace