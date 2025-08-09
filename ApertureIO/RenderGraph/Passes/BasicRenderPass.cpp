#include "ApertureIO/BasicRenderPass.hpp"

#include <vector>

namespace Aio
{

BasicRenderPass::BasicRenderPass(RenderGraph* renderGraph)
{
    /* RenderPass Details */
    _name = "BasicRenderPass";
    _type = RenderPassType::Graphics;

    /* Resources Access */
    ResourceAccess vertexBufferAccess{};
    vertexBufferAccess.name = "vertexBuffer";
    vertexBufferAccess.type = ResourceType::Vertex;
    vertexBufferAccess.access = AccessType::Read;
    vertexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(vertexBufferAccess);

    ResourceAccess indexBufferAccess{};
    indexBufferAccess.name = "indexBuffer";
    indexBufferAccess.type = ResourceType::Index;
    indexBufferAccess.access = AccessType::Read;
    indexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(indexBufferAccess);

    ResourceAccess uniformBufferAccess{};
    uniformBufferAccess.name = "uniformBuffer";
    uniformBufferAccess.type = ResourceType::Uniform;
    uniformBufferAccess.access = AccessType::Both;
    uniformBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(uniformBufferAccess);
    
    ResourceAccess textureBufferAccess{};
    textureBufferAccess.name = "basicTexture";
    textureBufferAccess.type = ResourceType::Texture;
    textureBufferAccess.access = AccessType::Read;
    textureBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(textureBufferAccess);

    /* Create Shader */
    ShaderCreateInfo BasicShaderCreateInfo{};
    BasicShaderCreateInfo.type = ShaderType::Graphics;
    BasicShaderCreateInfo.pContext = renderGraph->GetContextPtr();
    BasicShaderCreateInfo.pDevice = renderGraph->GetDevicePtr();
    BasicShaderCreateInfo.shaderName = "Basic Shader";
    BasicShaderCreateInfo.sourceFilepath = "./Shaders/Basic.glsl";
    _pShader = Shader::CreateShader(BasicShaderCreateInfo);
};

void BasicRenderPass::InitialiseResources(RenderGraph* renderGraph)
{
    /* Vertex Layout */
    BufferElement positionElement{};
    positionElement.count = 3;
    positionElement.type = BufferElementType::Float;
    positionElement.normalized = false;

    BufferElement colourElement{};
    colourElement.count = 3;
    colourElement.type = BufferElementType::Float;
    colourElement.normalized = false;

    BufferElement uvElement{};
    uvElement.count = 3;
    uvElement.type = BufferElementType::Float;
    uvElement.normalized = false;

    BufferLayout vertexLayout;
    vertexLayout.AddBufferElement(positionElement);
    vertexLayout.AddBufferElement(colourElement);
    vertexLayout.AddBufferElement(uvElement);

    //TODO: use a different type instead of the vector.
    // As we want to be able to have vec3s and vec2s in the same array
    // as we will be able to find them correctly with the bufferLayout
    
    /* Vertex Data */
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

    /* Vertex CreateInfo */
    BufferCreateInfo vertexInfo{};
    vertexInfo.context = renderGraph->GetContextPtr();
    vertexInfo.device = renderGraph->GetDevicePtr();
    vertexInfo.type = BufferType::Vertex;
    vertexInfo.layout = vertexLayout;
    vertexInfo.data = vertices.data();
    vertexInfo.count = static_cast<uint32_t>(vertices.size() / vertexLayout.GetElementCount());

    /* Create Vertex Buffer  */
    Buffer* vertexBuffer = Buffer::CreateBuffer(&vertexInfo);
    renderGraph->StoreBufferPtr("vertexBuffer", vertexBuffer);

    BufferElement indexElement{};
    indexElement.count = 1;
    indexElement.type = Aio::BufferElementType::Int;
    indexElement.normalized = false;

    /* Index Layout */
    BufferLayout indexLayout;
    indexLayout.AddBufferElement(indexElement);

    /* Index Data */
    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(3);

    /* Index CreateInfo */
    BufferCreateInfo indexInfo{};
    indexInfo.type = BufferType::Index;
    indexInfo.context = renderGraph->GetContextPtr();;
    indexInfo.device = renderGraph->GetDevicePtr();
    indexInfo.data = indices.data();
    indexInfo.layout = indexLayout;
    indexInfo.count = static_cast<uint32_t>(indices.size());

    /* Create Index Buffer  */
    Buffer* indexBuffer = Buffer::CreateBuffer(&indexInfo);
    renderGraph->StoreBufferPtr("indexBuffer", indexBuffer);

    /* Uniform Layout*/
    BufferElement floatElement{};
    floatElement.count = 1;
    floatElement.type = BufferElementType::Float;
    floatElement.normalized = false;

    BufferLayout uniformLayout;
    uniformLayout.AddBufferElement(floatElement);

    /* Uniform CreateInfo */
    BufferCreateInfo uniformInfo{};
    uniformInfo.type = BufferType::Uniform;
    uniformInfo.context = renderGraph->GetContextPtr();
    uniformInfo.device = renderGraph->GetDevicePtr();
    uniformInfo.data = nullptr;
    uniformInfo.layout = uniformLayout;
    uniformInfo.count = 1;

    /* Create Uniform Buffer and Upload Data */
    Buffer* uniformBuffer = Buffer::CreateBuffer(&uniformInfo);
    _uniformData.a = 1.0f;
    uniformBuffer->UploadToDevice(&_uniformData);
    renderGraph->StoreBufferPtr("uniformBuffer", uniformBuffer);
    
    /* Texture CreateInfo */
    Aio::TextureCreateInfo basicTextureInfo{};
    basicTextureInfo.context = renderGraph->GetContextPtr();
    basicTextureInfo.device = renderGraph->GetDevicePtr();
    basicTextureInfo.filePath = "./Textures/1K_Test_PNG_Texture.png";

    /* Create Texture */
    Texture* basicTexture = Texture::CreateTexture(&basicTextureInfo);
    renderGraph->StoreTexturePtr("basicTexture", basicTexture);
    
    _startTime = std::chrono::system_clock::now();
};

void BasicRenderPass::PreExecutePass(RenderGraph* renderGraph)
{
    renderGraph->GetTargetFrameBufferPtr()->Bind(_pRenderContext);
    renderGraph->GetBufferPtr("vertexBuffer")->Bind(_pRenderContext);
    renderGraph->GetBufferPtr("indexBuffer")->Bind(_pRenderContext);
    renderGraph->GetBufferPtr("uniformBuffer")->Bind(_pRenderContext);
    renderGraph->GetTexturePtr("basicTexture")->Bind(_pRenderContext);
    _pShader->Bind(_pRenderContext);
};

void BasicRenderPass::ExecutePass(RenderGraph* renderGraph)
{
    renderGraph->GetDevicePtr()->pCommand->Draw(_pRenderContext);
    updateUniformData(renderGraph);
};

void BasicRenderPass::updateUniformData(RenderGraph* renderGraph)
{
    std::chrono::duration<float> time = std::chrono::system_clock::now() - _startTime;
    float factor = 0.5f;
    float result = 0.5f * (std::sin(2 * PI * factor * time.count()) + 1.0);
    _uniformData.a = result;
    renderGraph->GetBufferPtr("uniformBuffer")->UploadToDevice(&_uniformData);
};

};