#ifndef H_VULKANCONTEXT
#define H_VULKANCONTEXT

#include <vector>

#include "../Base/Context.hpp"
#include "VulkanDevice.hpp"

#include "VulkanCommon.hpp"

/* TODO
    Include Volk and init this. 
    Include Vk-bootstrap 
    Use vk-bootstrap and start an vulkan instance
    */

namespace ApertureIO {

class VulkanContext : public Context
{
    public:
    void init() override; // TODO replace void to bool so we can check that the init errored!
    
    /* Getter Functions */
    VkInstance getVkInstance(); //TODO Should be a pointer to the instance
    
    private:
    std::vector<const char*> getRequiredExtensions();

    vkb::Instance _instance; // holds the vulkan instance
    friend class VulkanDevice;
};

} // End ApertureIO namespace

#endif