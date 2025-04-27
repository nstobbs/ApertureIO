#ifndef H_CONTEXT
#define H_CONTEXT
#include <iostream>

#include "../App/Window.hpp"

namespace ApertureIO {

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

    void setRendererAPI(RendererAPI API); //TODO will need to hook this up better to creating an context when this has't been set
    void setActiveWindow(Window* window);

    virtual void init() = 0;
    
    private:
    Window* _pActiveWindow;
    bool _started;
    RendererAPI _rendererAPI = eVulkan;
};
} //End ApertureIO namespace

#endif
