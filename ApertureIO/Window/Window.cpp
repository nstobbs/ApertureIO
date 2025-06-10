#include "Window.hpp"
#include "../Common/Logger.hpp"
#include <iostream>


namespace Aio {

Window::Window()
{
    Logger::LogInfo("Init Glfw");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _pWindow = glfwCreateWindow(_width, _height, "Don't Care", NULL, NULL);
};

GLFWwindow* Window::getWindowPtr()
{
    return _pWindow;
};

Window::~Window()
{
    glfwDestroyWindow(_pWindow);
    glfwTerminate();
}

} // End Aio namespace