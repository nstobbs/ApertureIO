#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

#include <vector>
#include <utility>

//TODO: Add Resize Support...

namespace TestApplication 
{

class Window
{
public:
    virtual std::pair<const char**, uint32_t> GetRequiredInstanceExtensions() = 0;
    virtual VkSurfaceKHR GetVkSurface() = 0;
    
    virtual void SetActiveFrameBuffer(Aio::FrameBuffer* framebuffer) = 0; 

    protected:
    Aio::VulkanFrameBuffer* _pFramebuffer;
};

}