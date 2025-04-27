#include <iostream>
#include "App/Window.hpp"
#include "Base/Context.hpp"
#include "Base/Device.hpp"
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

    /* The Device handles all*/
    ApertureIO::Device* GPUDevice = ApertureIO::Device::CreateDevice(GPUContext);
    if (!GPUDevice->init())
    {
        std::cout << "failed to start ApertureIO::Device :(\n";
    }

    // Main Loop Stuff Happens Here!
    while(!glfwWindowShouldClose(Window.getWindowPtr()))
    {
        std::cout << "running :)\n";
    };


    return EXIT_SUCCESS;
};