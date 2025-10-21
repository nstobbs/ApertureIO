#pragma once

#include "Window/Window.hpp"

#include "ApertureIO/Pointers.hpp"
#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanContext.hpp"

#include <string>
#include <vector>

#include <QVulkanWindow>

const int WIDTH = 640;
const int HIGHT = 480;
const std::string TITLE = "TestApplication Main QtWindow";

namespace TestApplication {

class WindowQtImpl : public Window
{
public:
    WindowQtImpl(Aio::Context* context);

    std::pair<const char**, uint32_t> GetRequiredInstanceExtensions() override;
    VkSurfaceKHR GetVkSurface() override;

    void SetActiveFrameBuffer(Aio::FrameBuffer* framebuffer) override;

private:
    
    UniquePtr<QVulkanWindow> qWindow;
    Aio::Context* _pContext;
    VkSurfaceKHR _surface = {VK_NULL_HANDLE};
};

}