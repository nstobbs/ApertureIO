#include "Device.hpp"
#include "../Vulkan/VulkanDevice.hpp"

namespace ApertureIO {

 Device* Device::CreateDevice(Context* context)
 {
    switch(context->getRendererAPI())
    {
        case eVulkan:
            return new VulkanDevice(context);
    }
 };

} // End ApertureIO namespace