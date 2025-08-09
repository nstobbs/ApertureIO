#pragma once

#include <iostream>

namespace Aio {

enum class RendererAPI {
    eNone = 0,
    eVulkan = 1
};

class Context
{
public:
    static Context* CreateContext();
    void setRendererAPI(RendererAPI API); //TODO will need to hook this up better to creating an context when this has't been set

    RendererAPI getRendererAPI();

    uint32_t getMaxFramesInFlight();
    uint32_t getCurrentFrame();
    void nextFrame();
    void setMaxFramesInFlight(uint32_t value);

    virtual void init() = 0;

private:
    bool _started;
    RendererAPI _rendererAPI = RendererAPI::eVulkan;
    uint32_t MAX_FRAMES_IN_FLIGHT = 2; // TODO: Feel like this should be a const in Context or something...
    uint32_t CurrentFrame = 0;
};
} //End Aio namespace