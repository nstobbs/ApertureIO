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
    VkInstance* GetVkInstance();
    vkb::Instance* GetBootstrapInstance();
    shaderc::Compiler* GetShadercCompiler();
    
    void SetRequiredExtensions(const char** extensions, uint32_t count);

private:

    UniquePtr<vkb::Instance> _instance;
    UniquePtr<shaderc::Compiler> _compiler;
    /* Window Extension*/
    const char** _requiredExtensions;
    uint32_t _requiredExtensionsCount;
    
};

} // End Aio namespace