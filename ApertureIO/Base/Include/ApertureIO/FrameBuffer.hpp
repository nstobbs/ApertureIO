#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Device.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace Aio {

class Device;
class RenderContext;

struct FrameBufferCreateInfo
{
    std::string name;
    bool isSwapChain;
    Device*  pDevice;
    Context* pContext;

    /* Required if isSwapChain = false */
    uint32_t height; 
    uint32_t width;
};

//TODO Do you want to mix color and depth formats with data formating?
enum class FrameBufferPixelFormat
{
    NONE_PIXEL_FORMAT = 0,
    COLOR_RGBA_8888 = 1,
    COLOR_RGBA_16161616_sFloat = 2, /* For gNormals */
    DEPTH_D32_S8 = 3,
};

class FrameBuffer
{
public:
    static UniquePtr<FrameBuffer> CreateFrameBuffer(const FrameBufferCreateInfo& createInfo);

    virtual void Bind(RenderContext& renderContext, bool isTarget) = 0;
    virtual void Unbind() = 0;

    void CreateLayer(const std::string& name, FrameBufferPixelFormat format);
    FrameBufferPixelFormat GetLayerPixelFormat(const std::string& name);
    std::vector<std::string> GetLayerNames();

    uint32_t GetWidth();
    uint32_t GetHeight();

protected:
    std::string _name;
    bool isSwapChainTarget = false;

    uint32_t _width;
    uint32_t _height;

    /* Layers */
    std::unordered_map<std::string, FrameBufferPixelFormat> _layersMap;
    std::vector<std::string> _layerOrder;
    uint32_t _layerCount = 0;
};

} // End Aio namespace