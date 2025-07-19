#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"

#include <vector>
#include <utility>

namespace TestApplication 
{

class Window
{
public:
    virtual std::pair<const char**, uint32_t> GetRequiredInstanceExtensions() = 0;
    virtual VkSurfaceKHR GetVkSurface() = 0;   
};

}