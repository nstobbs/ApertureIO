#pragma once

#include <vector>

#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanCommon.hpp"

namespace Aio {

class VulkanContext : public Context
{
    public:
    void init() override; // TODO replace void to bool so we can check that the init errored!
    
    /* Getter Functions */
    VkInstance GetVkInstance(); //TODO Should be a pointer to the instance
    shaderc_compiler_t GetShadercCompiler();
    
    private:
    std::vector<const char*> getRequiredExtensions();

    vkb::Instance _instance; // holds the vulkan instance
    shaderc_compiler_t _compiler;
    friend class VulkanDevice;
    
};

} // End Aio namespace