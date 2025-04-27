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

/* Setting Function */
void Context::setRendererAPI(RendererAPI API)
{
    _rendererAPI = API;
};

void Context::setActiveWindow(Window* window)
{
    _pActiveWindow = window;
};


} // End ApertureIO namespace