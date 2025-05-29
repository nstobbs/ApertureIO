#include "ApertureIO.hpp"

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>

int main()
{
    Aio::Logger::LogWarn("Logger Tests 1...");
    Aio::Logger::LogInfo("Logger Tests 2...");
    Aio::Logger::LogError("Logger Tests 3...");

    /* Window to Render to...*/
    Aio::Window Window;
    
    /* The Context handles all of the instance loading, extensions, validation layers and other stuff related to the
    selected graphics api */
    Aio::Context* context = Aio::Context::CreateContext();
    context->setActiveWindow(&Window);
    context->setRendererAPI(Aio::eVulkan); // TODO current this isn't needed for this system rn
    context->init();

    /* We will need to have a window created before we 
    can start creating the the GPUDevice */

    /* The Device handles all of the commands, memory, processing of the selected graphics api*/
    Aio::Device* GPU = Aio::Device::CreateDevice(context);
    if (!GPU->init())
    {
        std::cout << "failed to start Aio::Device :(\n";
        return EXIT_FAILURE;
    }

    /* The FrameBuffer is a render Target for any shaders to render to. This can be hook with an window for rendering
    to the screen directly. */
    Aio::FrameBuffer* framebuffer = Aio::FrameBuffer::CreateFrameBuffer(GPU, context);
    char* name = "SwapChain_FrameBuffer1";
    framebuffer->setName(name);
    if (!framebuffer->init()) //TODO: Does a FrameBuffer need a Init Function?
    {
        std::cout << "failed to create Aio::FrameBuffer :(\n";
        return EXIT_FAILURE;
    }

    /* Define the Buffer Layout And Create a Buffer to store and handle this data.*/
    /* Vertex Buffer*/
    Aio::BufferElement positionElement{};
    positionElement.count = 3;
    positionElement.type = Aio::Float;
    positionElement.normalized = false;

    Aio::BufferLayout vertexLayout;
    vertexLayout.AddBufferElement(positionElement);
    
    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    Aio::BufferCreateInfo bufferInfo{};
    bufferInfo.type = Aio::BufferType::Vertex;
    bufferInfo.context = context;
    bufferInfo.device = GPU;
    bufferInfo.data = vertices.data();
    bufferInfo.layout = vertexLayout;
    bufferInfo.count = static_cast<uint32_t>(vertices.size());

    Aio::Buffer* vertexbuffer = Aio::Buffer::CreateBuffer(&bufferInfo);

    /* Index Buffer*/
    Aio::BufferElement indexElement{};
    indexElement.count = 1;
    indexElement.type = Aio::Int;
    indexElement.normalized = false;

    Aio::BufferLayout indexLayout;
    indexLayout.AddBufferElement(indexElement);

    std::vector<int> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    Aio::BufferCreateInfo indexBufferInfo{};
    indexBufferInfo.type = Aio::BufferType::Index;
    indexBufferInfo.context = context;
    indexBufferInfo.device = GPU;
    indexBufferInfo.data = indices.data();
    indexBufferInfo.layout = indexLayout;
    indexBufferInfo.count = static_cast<uint32_t>(indices.size());

    Aio::Buffer* indexbuffer = Aio::Buffer::CreateBuffer(&indexBufferInfo);

    Aio::ShaderCreateInfo SolidColourShaderInfo{};
    SolidColourShaderInfo.type = Aio::ShaderType::Graphics;
    SolidColourShaderInfo.pContext = context;
    SolidColourShaderInfo.pDevice = GPU;
    SolidColourShaderInfo.shaderName = "SolidColour";
    SolidColourShaderInfo.sourceFilepath = "./Shaders/SolidColour.glsl";

    auto source = Aio::FileIO::ReadSourceFile("./Shaders/SolidColour.glsl");
    //auto sourceVert = Aio::FileIO::SplitOutShader(source, "#TYPE VERTEX");
    //auto sourceFrag = Aio::FileIO::SplitOutShader(source, "#TYPE FRAGMENT");
    //auto sourceComp = Aio::FileIO::SplitOutShader(source, "#TYPE COMPUTE");

    // Main Loop Stuff Happens Here!
    while(!glfwWindowShouldClose(Window.getWindowPtr()))
    {
        /* Start*/
        Aio::Logger::LogInfo("Running");
        return EXIT_SUCCESS;
        /* End */
    };

    return EXIT_SUCCESS;
};