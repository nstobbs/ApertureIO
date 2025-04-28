#include <iostream>
#include "App/Window.hpp"
#include "Base/Context.hpp"
#include "Base/Device.hpp"
#include "Base/FrameBuffer.hpp"
#include "GLFW/glfw3.h"

int main()
{
    /* Window to Render to...*/
    ApertureIO::Window Window;
    
    /* The Context handles all of the instance loading, extensions, validation layers and other stuff related to the
    selected graphics api */
    ApertureIO::Context* GPUContext = ApertureIO::Context::CreateContext();
    GPUContext->setActiveWindow(&Window);
    GPUContext->setRendererAPI(ApertureIO::eVulkan); // TODO current this isn't needed for this system rn
    GPUContext->init();

    /* We will need to have a window created before we 
    can start creating the the GPUDevice */

    /* The Device handles all of the commands, memory, processing of the selected graphics api*/
    ApertureIO::Device* GPUDevice = ApertureIO::Device::CreateDevice(GPUContext);
    if (!GPUDevice->init())
    {
        std::cout << "failed to start ApertureIO::Device :(\n";
        return EXIT_FAILURE;
    }

    /* The FrameBuffer is a render Target for any shaders to render to. This can be hook with an window for rendering
    to the screen directly. */
    ApertureIO::FrameBuffer* GPUFrameBuffer = ApertureIO::FrameBuffer::CreateFrameBuffer(GPUDevice, GPUContext);
    if (!GPUFrameBuffer->init()) //TODO: Does a FrameBuffer need a Init Function?
    {
        std::cout << "failed to create ApertureIO::FrameBuffer :(\n";
        return EXIT_FAILURE;
    }

    // Main Loop Stuff Happens Here!
    while(!glfwWindowShouldClose(Window.getWindowPtr()))
    {
        std::cout << "running :)\n";
    };


    return EXIT_SUCCESS;
};