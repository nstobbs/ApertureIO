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

    VkDevice _logicalDevice;
    VkPhysicalDevice _phyiscalDevice;
};

} // End namespace ApertureIO
#endif