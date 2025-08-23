#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanContext.hpp"

namespace Aio {

UniquePtr<Context> Context::CreateContext()
{   
    RendererAPI API = RendererAPI::eVulkan; 
    switch(API)
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanContext>(VulkanContext());
        case RendererAPI::eNone:
            std::runtime_error("RendererAPI was't selected during Context creation!");
    };
};

/* Getter Function */
RendererAPI Context::getRendererAPI()
{
    return _rendererAPI;
};

uint32_t Context::getCurrentFrame()
{
    return CurrentFrame;
};

void Context::nextFrame()
{
    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

uint32_t Context::getMaxFramesInFlight()
{
    return MAX_FRAMES_IN_FLIGHT;
};

/* Setting Function */
void Context::setRendererAPI(const RendererAPI API)
{
    _rendererAPI = API;
};


} // End Aio namespace