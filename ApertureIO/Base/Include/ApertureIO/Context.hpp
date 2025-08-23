#pragma once

#include "ApertureIO/Pointers.hpp"
#include <iostream>

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

namespace Aio {

enum class RendererAPI {
    eNone = 0,
    eVulkan = 1
};

class Context
{
public:
    static UniquePtr<Context> CreateContext();
    void setRendererAPI(const RendererAPI API); //TODO will need to hook this up better to creating an context when this has't been set

    RendererAPI getRendererAPI();

    uint32_t getMaxFramesInFlight();
    uint32_t getCurrentFrame();
    void nextFrame();

    virtual void init() = 0;

private:
    bool _started;
    RendererAPI _rendererAPI = RendererAPI::eVulkan; //FIXME xD
    uint32_t CurrentFrame = 0;
};
} //End Aio namespace