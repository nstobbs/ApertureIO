#pragma once

#include "ApertureIO/Context.hpp"
#include <volk.h>
#include <vector>

class Window
{
public:
    Window();
    ~Window();

    static Window* CreateWindow();

    virtual vector<char> GetRequiredExtensions() = 0;
    virtual VkSurfaceKHR GetVkSurface() = 0;   
}