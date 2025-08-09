// Aio::Common
#include "ApertureIO/FileIO.hpp"
#include "ApertureIO/Logger.hpp"

// Aio::Base
#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/BufferLayout.hpp"
#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Texture.hpp"
#include "ApertureIO/RenderContext.hpp"

// Aio::Vulkan
#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/VulkanDevice.hpp"

// Aio::RenderGraph
#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/BasicRenderPass.hpp"

//TestApplication 
#include "Window/WindowGLFWImpl.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cmath>
#include <chrono>
#include <ctime>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

int main()
{   
    /* The Context handles all of the instance loading, extensions, validation layers and other stuff related to the
    selected graphics api */
    //std::shared_ptr<Aio::Context> context = std::make_shared<Aio::Context>(Aio::Context::CreateContext());
    Aio::Context* context = Aio::Context::CreateContext();
    context->setRendererAPI(Aio::RendererAPI::eVulkan); // TODO current this isn't needed for this system rn

    /* Window to Render to...*/
    TestApplication::WindowGLFWImpl* window = new TestApplication::WindowGLFWImpl(context);

    auto extensions = window->GetRequiredInstanceExtensions();
    dynamic_cast<Aio::VulkanContext*>(context)->SetRequiredExtensions(extensions.first, extensions.second);
    context->init();

    /* The Device handles all of the commands, memory, processing of the selected graphics api*/
    Aio::Device* GPU = Aio::Device::CreateDevice(context);
    /* We will need to have a window created before we 
    can start creating the the GPUDevice. So we can set the surface and 
    give the GPU selector the right extensions */
    dynamic_cast<Aio::VulkanDevice*>(GPU)->SetVkSurfaceKHR(window->GetVkSurface()); // hack to set the vulkan surface and extensions for now.

    if (!GPU->init())
    {
        std::cout << "failed to start Aio::Device :(\n";
        return EXIT_FAILURE;
    }
    /* The FrameBuffer is a render Target for any shaders to render to. This can be hook with an window for rendering
    to the screen directly. */
    Aio::FrameBuffer* framebuffer = Aio::FrameBuffer::CreateFrameBuffer(GPU, context);
    std::string name = "SwapChain_FrameBuffer1";
    framebuffer->setName(name);
    if (!framebuffer->init()) //TODO: Does a FrameBuffer need a Init Function?
    {
        std::cout << "failed to create Aio::FrameBuffer :(\n";
        return EXIT_FAILURE;
    }
    window->SetActiveFrameBuffer(framebuffer);

    /* RenderGraph...*/
    Aio::RenderGraph renderGraph(GPU, context, framebuffer);

    /* Build Graph */
    Aio::BasicRenderPass basicPass(&renderGraph);
    renderGraph.AppendRenderPass(&basicPass);
    
    // Main Loop Stuff Happens Here!
    Aio::Logger::LogInfo("Running!");
    renderGraph.CompileGraph();

    while(!window->shouldClose())
    {
        /* Start*/
        glfwPollEvents();
        renderGraph.RenderFrame();
        /* End */
    };

    return EXIT_SUCCESS;
};