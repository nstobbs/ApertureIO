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

const float PI = 3.14f;

struct TestUniformStuct
{
    float a;
};

int main()
{
    Aio::Logger::LogWarn("Logger Tests 1...");
    Aio::Logger::LogInfo("Logger Tests 2...");
    Aio::Logger::LogError("Logger Tests 3...");

    
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

    /* Define the Buffer Layout And Create a Buffer to store and handle this data.*/
    /* Vertex Buffer*/
    Aio::BufferElement positionElement{};
    positionElement.count = 3;
    positionElement.type = Aio::BufferElementType::Float;
    positionElement.normalized = false;

    Aio::BufferElement colourElement{};
    colourElement.count = 3;
    colourElement.type = Aio::BufferElementType::Float;
    colourElement.normalized = false;

    Aio::BufferElement uvElement{};
    uvElement.count = 3;
    uvElement.type = Aio::BufferElementType::Float;
    uvElement.normalized = false;

    Aio::BufferLayout vertexLayout;
    vertexLayout.AddBufferElement(positionElement);
    vertexLayout.AddBufferElement(colourElement);
    vertexLayout.AddBufferElement(uvElement);
    
    //TODO: use a different type instead of the vector.
    // As we want to be able to have vec3s and vec2s in the same array
    // as we will be able to find them correctly with the bufferLayout

    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // ColourElement
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // uvElement

    vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // ColourElement
    vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // uvElement
    
    vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // ColourElement
    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // uvElement

    vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f)); // PositionElement
    vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f)); // ColourElement
    vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // uvElement

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
    indexElement.type = Aio::BufferElementType::Int;
    indexElement.normalized = false;

    Aio::BufferLayout indexLayout;
    indexLayout.AddBufferElement(indexElement);

    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(3);

    Aio::BufferCreateInfo indexBufferInfo{};
    indexBufferInfo.type = Aio::BufferType::Index;
    indexBufferInfo.context = context;
    indexBufferInfo.device = GPU;
    indexBufferInfo.data = indices.data();
    indexBufferInfo.layout = indexLayout;
    indexBufferInfo.count = static_cast<uint32_t>(indices.size());

    Aio::Buffer* indexbuffer = Aio::Buffer::CreateBuffer(&indexBufferInfo);

    /* This is One Way to Handle Uniform Buffers.  
    For Multiple Shaders with one Buffer Object.*/

    Aio::BufferElement floatElement{};
    floatElement.count = 1;
    floatElement.type = Aio::BufferElementType::Float;
    floatElement.normalized = false;

    Aio::BufferLayout uniformLayout;
    uniformLayout.AddBufferElement(floatElement);

    Aio::BufferCreateInfo uniformBufferInfo{};
    uniformBufferInfo.type = Aio::BufferType::Uniform;
    uniformBufferInfo.context = context;
    uniformBufferInfo.device = GPU;
    uniformBufferInfo.data = nullptr;
    uniformBufferInfo.layout = uniformLayout;
    uniformBufferInfo.count = 1;

    Aio::Buffer* uniformBuffer = Aio::Buffer::CreateBuffer(&uniformBufferInfo);
    TestUniformStuct testUniformData{};
    testUniformData.a = 1.0f;
    uniformBuffer->UploadToDevice(&testUniformData);

    /* Create an Texture to be used */
    Aio::TextureCreateInfo testTextureInfo{};
    testTextureInfo.context = context;
    testTextureInfo.device = GPU;
    testTextureInfo.filePath = "./Textures/1K_Test_PNG_Texture.png";
    Aio::Texture* testTexture = Aio::Texture::CreateTexture(testTextureInfo);

    /* Create an Shader Program to Run */
    Aio::ShaderCreateInfo BasicShaderCreateInfo{};
    BasicShaderCreateInfo.type = Aio::ShaderType::Graphics;
    BasicShaderCreateInfo.pContext = context;
    BasicShaderCreateInfo.pDevice = GPU;
    BasicShaderCreateInfo.shaderName = "Basic Shader";
    BasicShaderCreateInfo.sourceFilepath = "./Shaders/Basic.glsl";

    Aio::ShaderLibrary shaderLibrary("./Shaders/");
    Aio::Shader* BasicShader = Aio::Shader::CreateShader(BasicShaderCreateInfo);
    shaderLibrary.AddShader(BasicShader);

    /* Here's another way to use the Uniform Buffer per Shader
    without creating an Buffer. The Uniform buffer is managed 
    inside of the Shader. All Uniforms should be set before 
    rendering. */
    //BasicShader->SetFloat("basic_float", 1.0f);

    /* Bind all the objects needed to the RenderContext */
    Aio::RenderContext rContext;

    vertexbuffer->Bind(rContext);
    indexbuffer->Bind(rContext);
    uniformBuffer->Bind(rContext);
    framebuffer->Bind(rContext);
    BasicShader->Bind(rContext);
    testTexture->Bind(rContext);

    // Main Loop Stuff Happens Here!
    Aio::Logger::LogInfo("Running!");
    auto startTime = std::chrono::system_clock::now();

    while(!window->shouldClose())
    {
        /* Start*/
        glfwPollEvents();

        // Test Updating Uniform Data
        std::chrono::duration<float> time = std::chrono::system_clock::now() - startTime;
        float factor = 0.5f;
        float result = 0.5f * (std::sin(2 * PI * factor * time.count()) + 1.0);
        testUniformData.a = result;
        uniformBuffer->UploadToDevice(&testUniformData);

        GPU->pCommand->BeginFrame(rContext);
        GPU->pCommand->Draw(rContext);
        GPU->pCommand->EndFrame(rContext);
        /* End */
    };

    return EXIT_SUCCESS;
};