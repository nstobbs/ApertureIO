#ifndef H_WINDOW
#define H_WINDOW

#include <GLFW/glfw3.h>

/* This will be based of GLFW cause I don't care about qt.
 I'll worry about that later*/

namespace ApertureIO {

class Window
{
    public:
    Window();
    ~Window();

    GLFWwindow* getWindowPtr();

    private:
    //TODO need to make a cross-platform for data types. uint32_t is windows only
    uint32_t _width = 640;
    uint32_t _height = 480;
    GLFWwindow* _pWindow;
};

} // End ApertureIO namespace 

#endif