#include "VulkanContext.hpp"
#include <iostream>

namespace ApertureIO {

void VulkanContext::init()
{
    std::cout << "Init Vulkan :p\n";
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

    // build instance
    auto builderResult = builder.build();
    if (!builderResult)
    {
        std::cout << "failed to build a instance :(\n";
    }

    // volk loads all extension to start using them
    _instance = builderResult.value();
    volkLoadInstance(_instance);
};

VkInstance VulkanContext::getVkInstance()
{
    return _instance.instance;
}

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

} // End ApertureIO namespace