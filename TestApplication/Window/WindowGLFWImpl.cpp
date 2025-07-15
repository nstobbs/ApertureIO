#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"
#include "include/Window.hpp"

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

const int WIDTH = 1280;
const int HIGHT = 720;
const std::string TITLE = "TestApplication Main Window";

namespace TestApplication {

class WindowGLFWImpl : public Window
{
public:

    WindowGLFWImpl(Aio::Context* context) : _pContext(context)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _pWindow = glfwCreateWindow(WIDTH, HIGHT, TITLE.c_str(), NULL, NULL);
        if (!_pWindow)
        {
            Aio::Logger::LogError("_pWindow is a nullptr...");
            throw std::runtime_error("Closing");
        }
    };

    ~WindowGLFWImpl()
    {
        auto vulkanContext = dynamic_cast<Aio::VulkanContext*>(_pContext);
        vkDestroySurfaceKHR(vulkanContext->GetVkInstance(), _surface, nullptr);
    }

    std::vector<char> GetRequiredExtensions() override
    {
        uint32_t count;
        const std::vector<char> extensions = glfwGetRequiredInstanceExtensions(&count);
        return extensions;
    }
    
    VkSurfaceKHR GetVkSurface() override
    {
        if (!_surface)
        {
            VkInstance instance = dynamic_cast<Aio::VulkanContext*>(_pContext)->GetVkInstance();
            VK_ASSERT(glfwCreateWindowSurface(instance, _pWindow, nullptr, &_surface), VK_SUCCESS, "glfwCreateWindowSurface failed.");
        }
        return _surface;
    };

    bool shouldClose()
    {
        return glfwWindowShouldClose(_pWindow);
    };

private:
    GLFWwindow* _pWindow = {nullptr};
    Aio::Context* _pContext = {nullptr};
    VkSurfaceKHR _surface = {VK_NULL_HANDLE};
};

}

