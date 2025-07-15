#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Device.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace Aio {
    class Device;
    class Context;
    class RenderContext;
}

namespace Aio {

//TODO Do you want to mix color and depth formats with data formating?
enum FrameBufferPixelFormat
{
    NONE_PIXEL_FORMAT = 0,
    COLOR_RGBA_8888 = 1,
    DEPTH_D32_S8 = 2,
};

class FrameBuffer
{
public:
    static FrameBuffer* CreateFrameBuffer(Device* pDevice, Context* pContext); // For SwapChain.
    static FrameBuffer* CreateFrameBuffer(Device* pDevice); // Default

    virtual bool init() = 0; // TODO Does a framebuffer need an init function?

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;

    void createLayer(std::string name, FrameBufferPixelFormat format);
    void setName(std::string name);

    friend class VulkanFrameBuffer; // TODO: not sure if I like this really.

protected:
    std::string _name;
    bool isSwapChainTarget = false;

    unsigned int _width;
    unsigned int _height;

    unsigned int _layerCount = 0;
    std::unordered_map<std::string, FrameBufferPixelFormat> _layers;
};

} // End Aio namespace