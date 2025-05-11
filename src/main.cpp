#include "ApertureIO.hpp"

#include <iostream>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <vector>

int main()
{
    /* Window to Render to...*/
    ApertureIO::Window Window;
    
    /* The Context handles all of the instance loading, extensions, validation layers and other stuff related to the
    selected graphics api */
    ApertureIO::Context* context = ApertureIO::Context::CreateContext();
    context->setActiveWindow(&Window);
    context->setRendererAPI(ApertureIO::eVulkan); // TODO current this isn't needed for this system rn
    context->init();

    /* We will need to have a window created before we 
    can start creating the the GPUDevice */

    /* The Device handles all of the commands, memory, processing of the selected graphics api*/
    ApertureIO::Device* gpu = ApertureIO::Device::CreateDevice(context);
    if (!gpu->init())
    {
        std::cout << "failed to start ApertureIO::Device :(\n";
        return EXIT_FAILURE;
    }

    /* The FrameBuffer is a render Target for any shaders to render to. This can be hook with an window for rendering
    to the screen directly. */
    ApertureIO::FrameBuffer* framebuffer = ApertureIO::FrameBuffer::CreateFrameBuffer(gpu, context);
    char* name = "SwapChain_FrameBuffer1";
    framebuffer->setName(name);
    if (!framebuffer->init()) //TODO: Does a FrameBuffer need a Init Function?
    {
        std::cout << "failed to create ApertureIO::FrameBuffer :(\n";
        return EXIT_FAILURE;
    }

    /* Define the Buffer Layout And Create a Buffer to store and handle this data.*/
    /* Vertex Buffer*/
    ApertureIO::BufferElement positionElement{};
    positionElement.count = 3;
    positionElement.type = ApertureIO::Float;
    positionElement.normalized = false;

    ApertureIO::BufferLayout vertexLayout;
    vertexLayout.AddBufferElement(positionElement);
    
    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    ApertureIO::BufferCreateInfo bufferInfo{};
    bufferInfo.context = context;
    bufferInfo.device = gpu;
    bufferInfo.hostAccess = false;
    bufferInfo.data = vertices.data();
    bufferInfo.layout = vertexLayout;
    bufferInfo.count = static_cast<uint32_t>(vertices.size());

    ApertureIO::Buffer* vertexbuffer = ApertureIO::Buffer::CreateBuffer(&bufferInfo);

    /* Index Buffer*/
    ApertureIO::BufferElement indexElement{};
    indexElement.count = 1;
    indexElement.type = ApertureIO::Int;
    indexElement.normalized = false;

    ApertureIO::BufferLayout indexLayout;
    indexLayout.AddBufferElement(indexElement);

    std::vector<int> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    ApertureIO::BufferCreateInfo indexBufferInfo{};
    indexBufferInfo.context = context;
    indexBufferInfo.device = gpu;
    indexBufferInfo.hostAccess = false;
    indexBufferInfo.data = indices.data();
    indexBufferInfo.layout = indexLayout;
    indexBufferInfo.count = static_cast<uint32_t>(indices.size());

    ApertureIO::Buffer* indexbuffer = ApertureIO::Buffer::CreateBuffer(&indexBufferInfo);

    // Main Loop Stuff Happens Here!
    while(!glfwWindowShouldClose(Window.getWindowPtr()))
    {
        std::cout << "running :)\n";
    };

    return EXIT_SUCCESS;
};