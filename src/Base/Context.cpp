#include "Context.hpp"
#include "../Vulkan/VulkanContext.hpp"

namespace ApertureIO {

Context* Context::CreateContext()
{   
    RendererAPI API = RendererAPI::eVulkan; 
    switch(API)
    {
        case RendererAPI::eVulkan:
            return new VulkanContext();
        case RendererAPI::eNone:
            std::runtime_error("RendererAPI was't selected during Context creation!");
    };
};

/* Getter Function */
RendererAPI Context::getRendererAPI()
{
    return _rendererAPI;
};

Window* Context::getActiveWindowPtr()
{
    return _pActiveWindow;
};

uint32_t Context::getCurrentFrame()
{
    return CurrentFrame;
};

void Context::nextFrame()
{
    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

/* Setting Function */
void Context::setMaxFramesInFlight(uint32_t value)
{
    MAX_FRAMES_IN_FLIGHT = value;
};

void Context::setRendererAPI(RendererAPI API)
{
    _rendererAPI = API;
};

void Context::setActiveWindow(Window* window)
{
    _pActiveWindow = window;
};


} // End ApertureIO namespace