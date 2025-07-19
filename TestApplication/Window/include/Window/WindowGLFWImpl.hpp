#include "Window/Window.hpp"

#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"

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

    WindowGLFWImpl(Aio::Context* context);
    ~WindowGLFWImpl();

    
    std::pair<const char**, uint32_t> GetRequiredInstanceExtensions() override;
    VkSurfaceKHR GetVkSurface() override;

    bool shouldClose();

private:
    GLFWwindow* _pWindow = {nullptr};
    Aio::Context* _pContext = {nullptr};
    VkSurfaceKHR _surface = {VK_NULL_HANDLE};
};

}