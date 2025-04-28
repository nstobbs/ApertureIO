#ifndef H_VULKANDEVICE
#define H_VULKANDEVICE

#include "VulkanCommon.hpp"
#include "VulkanContext.hpp"
#include "../Base/Device.hpp"
#include "../Base/Context.hpp"

namespace ApertureIO {

class VulkanContext;

class VulkanDevice : public Device
{
    public:
    VulkanDevice(Context* context);
    bool init() override;

    private:

    VulkanContext* _pVulkanContext;

    vkb::PhysicalDevice _physicalDevice;
    vkb::Device _device;
};

} // End namespace ApertureIO
#endif