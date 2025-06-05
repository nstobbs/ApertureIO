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

    Aio::BufferElement colourElement{};
    colourElement.count = 3;
    colourElement.type = Aio::Float;
    colourElement.normalized = false;

    Aio::BufferLayout vertexLayout;
    vertexLayout.AddBufferElement(positionElement);
    vertexLayout.AddBufferElement(colourElement);
    
    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(0.0f, -0.5f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // ColourElement

    vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // ColourElement
    
    vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // ColourElement

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

    std::vector<uint32_t> indices;
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

    /* Create an Shader Program to Run */
    Aio::ShaderCreateInfo BasicShaderCreateInfo{};
    BasicShaderCreateInfo.type = Aio::ShaderType::Graphics;
    BasicShaderCreateInfo.pContext = context;
    BasicShaderCreateInfo.pDevice = GPU;
    BasicShaderCreateInfo.shaderName = "Basic Shader";
    BasicShaderCreateInfo.sourceFilepath = "./src/Shaders/Basic.glsl";

    Aio::ShaderManager shaderManager;
    Aio::Shader* BasicShader = Aio::Shader::CreateShader(BasicShaderCreateInfo);
    shaderManager.AddShader(BasicShader);

    /* Bind all the objects needed to the RenderContext */
    Aio::RenderContext rContext;

    vertexbuffer->Bind(rContext);
    indexbuffer->Bind(rContext);
    framebuffer->Bind(rContext);
    BasicShader->Bind(rContext);

    // Main Loop Stuff Happens Here!
    Aio::Logger::LogInfo("Running!");
    while(!glfwWindowShouldClose(Window.getWindowPtr()))
    {
        /* Start*/
        glfwPollEvents();
        GPU->pCommand->Draw(rContext);
        context->nextFrame();
        /* End */
    };

    return EXIT_SUCCESS;
};