// Aio::Common
#include "ApertureIO/FileIO.hpp"
#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Pointers.hpp"

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
#include "ApertureIO/RenderEngine.hpp"
#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/ReadAssimp.hpp"

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
    UniquePtr<Aio::Context> context = Aio::Context::CreateContext();
    context.get()->setRendererAPI(Aio::RendererAPI::eVulkan); // TODO current this isn't needed for this system rn

    /* Window to Render to...*/
    TestApplication::WindowGLFWImpl* window = new TestApplication::WindowGLFWImpl(context.get());

    auto extensions = window->GetRequiredInstanceExtensions();
    dynamic_cast<Aio::VulkanContext*>(context.get())->SetRequiredExtensions(extensions.first, extensions.second);
    context->init();

    /* The Device handles all of the commands, memory, processing of the selected graphics api*/
    UniquePtr<Aio::Device> GPU = Aio::Device::CreateDevice(context.get());
    /* We will need to have a window created before we 
    can start creating the the GPUDevice. So we can set the surface and 
    give the GPU selector the right extensions */
    dynamic_cast<Aio::VulkanDevice*>(GPU.get())->SetVkSurfaceKHR(window->GetVkSurface()); // hack to set the vulkan surface and extensions for now.

    if (!GPU.get()->init())
    {
        std::cout << "failed to start Aio::Device :(\n";
        return EXIT_FAILURE;
    }
    /* The FrameBuffer is a render Target for any shaders to render to. This can be hook with an window for rendering
    to the screen directly. */
    Aio::FrameBufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.isSwapChain = true;
    framebufferCreateInfo.name = "Main_SwapchainFrameBuffer";
    framebufferCreateInfo.pContext = context.get();
    framebufferCreateInfo.pDevice = GPU.get();
    
    UniquePtr<Aio::FrameBuffer> framebuffer = Aio::FrameBuffer::CreateFrameBuffer(framebufferCreateInfo);
    window->SetActiveFrameBuffer(framebuffer.get());

    /* RenderEngine */
    Aio::RenderEngine engine(GPU.get(), context.get(), framebuffer.get());
    UniquePtr<Aio::RenderGraph> graph = std::make_unique<Aio::RenderGraph>();

    /* Build Graph */
    auto read = graph->CreateRenderPass("ReadAssimp");
    auto lights = graph->CreateRenderPass("PhongLighting");
    auto asciiArt = graph->CreateRenderPass("AsciiImage");
    read->GetOutPort("image")->Connect(lights->GetInPort("image"));
    lights->GetOutPort("image")->Connect(asciiArt->GetInPort("image"));

    dynamic_cast<Aio::ReadAssimp*>(read)->ReadFile("./Models/viking_room.obj", "./Textures/viking_room.png");
    
    engine.LoadGraph("ReadModel", std::move(graph));
    engine.SetActive("ReadModel");
    
    while(!window->shouldClose())
    {
        /* Start*/
        glfwPollEvents();
        engine.ExecuteFrame();
        /* End */
    };

    return EXIT_SUCCESS;
};