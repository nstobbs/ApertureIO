#ifndef H_FRAMEBUFFER
#define H_FRAMEBUFFER

#include <vector>
#include <unordered_map>

#include "Context.hpp"
#include "Device.hpp"

namespace ApertureIO {

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
    
    void createLayer(char* name, FrameBufferPixelFormat format);

    private:
    char* name = "";
    bool isSwapChainTarget = false;

    // Surface
    unsigned int _width;
    unsigned int _height;

    // Layers
    unsigned int _layerCount;
    std::unordered_map<char*, FrameBufferPixelFormat> _layers;
};

} // End ApertureIO namespace

#endif