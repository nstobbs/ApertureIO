#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanCommon.hpp"

#include <vector>

namespace Aio {

class VulkanContext : public Context
{
public:
    void init() override; // TODO replace void to bool so we can check that the init errored!
    
    /* Getter Functions */
    VkInstance GetVkInstance(); //TODO Should be a pointer to the instance
    shaderc::Compiler* GetShadercCompiler();
    
    void SetRequiredExtensions(const char** extensions, uint32_t count);

private:

    vkb::Instance _instance; // holds the vulkan instance
    shaderc::Compiler* _compiler;

    friend class VulkanDevice; //TODO: Find a way to remove this...
    const char** _requiredExtensions;
    uint32_t _requiredExtensionsCount;
    
};

} // End Aio namespace