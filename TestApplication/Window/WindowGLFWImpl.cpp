#include "Window/WindowGLFWImpl.hpp"

namespace TestApplication {

static void windowResizedCallback(GLFWwindow* window, int width, int height)
{
    auto glfwWindowImpl = reinterpret_cast<WindowGLFWImpl*>(glfwGetWindowUserPointer(window));
    glfwWindowImpl->triggerFrameBufferRebuild();
}

WindowGLFWImpl::WindowGLFWImpl(Aio::Context* context)
{
    _pContext = context; 
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _pWindow = glfwCreateWindow(WIDTH, HIGHT, TITLE.c_str(), NULL, NULL);
    if (!_pWindow)
    {
        Aio::Logger::LogError("Failed to Create GLFW Window...");
        throw std::runtime_error("Closing");
    }

    glfwSetWindowUserPointer(_pWindow, this);
    glfwSetFramebufferSizeCallback(_pWindow, windowResizedCallback);
};

WindowGLFWImpl::~WindowGLFWImpl()
{
    glfwDestroyWindow(_pWindow);
    auto vulkanContext = dynamic_cast<Aio::VulkanContext*>(_pContext);
    vkDestroySurfaceKHR(vulkanContext->GetVkInstance(), _surface, nullptr);
};

std::pair<const char**, uint32_t> WindowGLFWImpl::GetRequiredInstanceExtensions()
{
    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    std::pair<const char**, uint32_t> result(extensions, count);
    return result;
};

VkSurfaceKHR WindowGLFWImpl::GetVkSurface()
{
    if (!_surface)
    {
        VkInstance instance = dynamic_cast<Aio::VulkanContext*>(_pContext)->GetVkInstance();
        VK_ASSERT(glfwCreateWindowSurface(instance, _pWindow, nullptr, &_surface), VK_SUCCESS, "glfwCreateWindowSurface failed.");
    }
    return _surface;
};

void WindowGLFWImpl::triggerFrameBufferRebuild()
{
    _pFramebuffer->Rebuild();
};


void WindowGLFWImpl::SetActiveFrameBuffer(Aio::FrameBuffer* framebuffer)
{
    _pFramebuffer = dynamic_cast<Aio::VulkanFrameBuffer*>(framebuffer); 
};

bool WindowGLFWImpl::shouldClose()
{
    return glfwWindowShouldClose(_pWindow);
};

}