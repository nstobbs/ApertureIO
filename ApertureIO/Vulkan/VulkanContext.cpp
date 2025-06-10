#include "VulkanContext.hpp"
#include <iostream>
#include "../Common/Logger.hpp"

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
    builder.enable_extensions(getRequiredExtensions());

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
    _compiler = shaderc_compiler_initialize();
    // TODO: create an teardown function for this class
};

VkInstance VulkanContext::GetVkInstance()
{
    return _instance.instance;
}

shaderc_compiler_t VulkanContext::GetShadercCompiler()
{
    return _compiler;
};

//TODO: this need to be rewritten so different platforms can imp they own!
std::vector<const char*> VulkanContext::getRequiredExtensions()
{
    uint32_t glfwExtCount;
    const char** glfwExtNames;
    glfwExtNames = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    std::vector<const char*> extensions(glfwExtNames, glfwExtNames +  glfwExtCount);

    //TODO check if i need to add validation extensions too as I think volk should
    // be dealing with it.

    return extensions;
};

} // End Aio namespace