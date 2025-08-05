#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/Logger.hpp"

#include <iostream>
#include <utility>

namespace Aio {

void VulkanContext::init()
{
    Logger::LogInfo("Init Vulkan");
    VkResult result = volkInitialize();
    if (result != VK_SUCCESS)
    {
        std::cout << "failed to init volk :(\n";
    }

    // vulkan instance creation
    vkb::InstanceBuilder builder;
    builder.set_app_name("Test");
    builder.set_engine_name("ApertureIO");
    builder.require_api_version(1,3,0);
    builder.enable_validation_layers();
    builder.enable_extensions(_requiredExtensionsCount, _requiredExtensions);

    // system info
    auto systemInfo = vkb::SystemInfo::get_system_info().value();
    if (systemInfo.is_extension_available("VK_KHR_get_physical_device_properties2"))
    {
        builder.enable_extension("VK_KHR_get_physical_device_properties2");
    };

    // build instance
    auto builderResult = builder.build();
    if (!builderResult)
    {
        std::cout << "failed to build a instance :(\n";
    }

    // volk loads all extension to start using them
    _instance = builderResult.value();
    volkLoadInstance(_instance);
    
    // boot up the shader compiler
    _compiler = new shaderc::Compiler();
    // TODO: create an teardown function for this class
};

void VulkanContext::SetRequiredExtensions(const char** extensions, uint32_t count)
{
    _requiredExtensions = extensions;
    _requiredExtensionsCount = count;
}

VkInstance VulkanContext::GetVkInstance()
{
    return _instance.instance;
};

shaderc::Compiler* VulkanContext::GetShadercCompiler()
{
    return _compiler;
};

} // End Aio namespace