#include "Window.hpp"
#include <iostream>


namespace Aio {

Window::Window()
{
    std::cout << "glfwInit\n";
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