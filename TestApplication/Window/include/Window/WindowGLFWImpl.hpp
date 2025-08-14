#pragma once

#include "Window/Window.hpp"

#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

const int WIDTH = 640;
const int HIGHT = 480;
const std::string TITLE = "TestApplication Main Window";

namespace TestApplication {

class WindowGLFWImpl : public Window
{
public:

    WindowGLFWImpl(WeakPtr<Context> context);
    ~WindowGLFWImpl();

    std::pair<const char**, uint32_t> GetRequiredInstanceExtensions() override;
    VkSurfaceKHR GetVkSurface() override;

    void SetActiveFrameBuffer(WeakPtr<Aio::FrameBuffer> framebuffer) override;

    void triggerFrameBufferRebuild();
    bool shouldClose();

private:
    SharedPtr<GLFWwindow> _pWindow;
    SharedPtr<Aio::Context> _pContext;
    VkSurfaceKHR _surface = {VK_NULL_HANDLE};
};

}