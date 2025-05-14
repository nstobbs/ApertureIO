#ifndef H_CONTEXT
#define H_CONTEXT
#include <iostream>

#include "../App/Window.hpp"

namespace Aio {

enum RendererAPI {
    eNone = 0,
    eVulkan = 1
};

class Context
{
    public:
    static Context* CreateContext();

    RendererAPI getRendererAPI();
    Window* getActiveWindowPtr();
    uint32_t getMaxFramesInFlight();

    uint32_t getCurrentFrame();
    void nextFrame();
    void setMaxFramesInFlight(uint32_t value);

    void setRendererAPI(RendererAPI API); //TODO will need to hook this up better to creating an context when this has't been set
    void setActiveWindow(Window* window);

    virtual void init() = 0;
    
    private:
    Window* _pActiveWindow;
    bool _started;
    RendererAPI _rendererAPI = eVulkan;
    uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t CurrentFrame = 0;
};
} //End Aio namespace

#endif
