#ifndef H_FRAMEBUFFER
#define H_FRAMEBUFFER

#include <vector>
#include <unordered_map>

#include "Context.hpp"
#include "Device.hpp"

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

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    
    void createLayer(char* name, FrameBufferPixelFormat format);
    void setName(char* name);

    friend class VulkanFrameBuffer; // TODO: not sure if I like this really.

    private:
    char* _name = "";
    bool isSwapChainTarget = false;

    unsigned int _width;
    unsigned int _height;

    unsigned int _layerCount = 0;
    std::unordered_map<char*, FrameBufferPixelFormat> _layers;
};

} // End Aio namespace

#endif