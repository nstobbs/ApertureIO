#include "ApertureIO/AsciiImage.hpp"

namespace Aio
{

const std::string ACTIVE_ASCII_PACK = "Pack2";

AsciiImage::AsciiImage()
{
    /* RenderPass Info */
    _name = "AsciiImage";
    _type = RenderPassType::Compute;

    /* Resources Access */
    ResourceAccess asciiImageAccess{};
    asciiImageAccess.name = "asciiImageSettings";
    asciiImageAccess.type = ResourceType::Uniform;
    asciiImageAccess.access = AccessType::Read;
    asciiImageAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(asciiImageAccess);

    ResourceAccess asciiImageTextureMap{};
    asciiImageTextureMap.name = "asciiImageTexture";
    asciiImageTextureMap.type = ResourceType::Texture;
    asciiImageTextureMap.access = AccessType::Read;
    asciiImageTextureMap.isInitialisingResource = true;
    _resourcesAccess.push_back(asciiImageTextureMap);

    ResourceAccess asciiImageOutAccess{};
    asciiImageOutAccess.name = "asciiImageOut";
    asciiImageOutAccess.type = ResourceType::FrameBuffer;
    asciiImageOutAccess.access = AccessType::Write;
    asciiImageAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(asciiImageOutAccess);

    /* 
    if (_inPorts.at("image").GetIncomingResourceType() == ResourceType::FrameBuffer)
    {
        ResourceAccess imageInAccess{};
        imageInAccess.name = _inPorts.at("image").GetIncomingResourceName();
        imageInAccess.type = ResourceType::FrameBuffer;
        imageInAccess.access = AccessType::Read;
        imageInAccess.isInitialisingResource = false;
        _resourcesAccess.push_back(imageInAccess);
    } */
    
    /* Ports */
    Port imageIn(this);
    Port imageOut(this);
    _inPorts.emplace("image", imageIn);
    _outPorts.emplace("image", imageOut);
};

void AsciiImage::BuildKnobs()
{
    
};

void AsciiImage::updateUniformBuffer(RenderEngine* renderEngine)
{
    renderEngine->GetBufferPtr("asciiImageSettings")->UploadToDevice(&_settings);
};

void AsciiImage::AllocateResources(RenderEngine*  renderEngine)
{
    /* Shader Allocation */
    ShaderCreateInfo shaderInfo{};
    shaderInfo.name = "AsciiImage";
    shaderInfo.pContext = renderEngine->GetContextPtr();
    shaderInfo.pDevice = renderEngine->GetDevicePtr();
    shaderInfo.type = ShaderType::Compute;
    shaderInfo.sourceFilepath = "./Shaders/AsciiImage.glsl";
    renderEngine->GetShaderLibraryPtr()->CreateShader(shaderInfo);
    _pShader = renderEngine->GetShaderLibraryPtr()->GetShader("AsciiImage");

    BufferLayout asciiImageSettingsLayout;
    BufferElement intElement{};
    intElement.count = 1;
    intElement.normalized = false;
    intElement.type = BufferElementType::Int;
    asciiImageSettingsLayout.AddBufferElement(intElement); // MapSize
    asciiImageSettingsLayout.AddBufferElement(intElement); // CharCount

    BufferElement floatElement{};
    floatElement.count = 1;
    floatElement.normalized = false;
    floatElement.type = BufferElementType::Float;
    asciiImageSettingsLayout.AddBufferElement(floatElement); // Brightness 

    BufferElement boolElement{};
    boolElement.count = 1;
    boolElement.normalized = false;
    boolElement.type = BufferElementType::Bool;
    asciiImageSettingsLayout.AddBufferElement(boolElement); // MatrixMode

    BufferCreateInfo asciiSettingsBufferInfo{};
    asciiSettingsBufferInfo.context = renderEngine->GetContextPtr();
    asciiSettingsBufferInfo.device = renderEngine->GetDevicePtr();
    asciiSettingsBufferInfo.count = 1;
    asciiSettingsBufferInfo.type = BufferType::Uniform;
    asciiSettingsBufferInfo.layout = asciiImageSettingsLayout;
    renderEngine->StoreBufferPtr("asciiImageSettings", Buffer::CreateBuffer(asciiSettingsBufferInfo));

    int numberOfChar = 9;
    for (int i = 0; i < numberOfChar; i++)
    {
        TextureCreateInfo asciiImageTextureInfo{};
        asciiImageTextureInfo.pContext = renderEngine->GetContextPtr();
        asciiImageTextureInfo.pDevice = renderEngine->GetDevicePtr();
        std::string filename = "Map_" + std::to_string(i) + ".png";
        asciiImageTextureInfo.filePath = "./Textures/AsciiImageTextures/" + ACTIVE_ASCII_PACK + "/" + filename;
        renderEngine->StoreTexturePtr(filename, Texture::CreateTexture(asciiImageTextureInfo));
    };

    FrameBufferCreateInfo asciiImageOutInfo{};
    asciiImageOutInfo.name = "asciiImageOut";
    asciiImageOutInfo.pContext = renderEngine->GetContextPtr();
    asciiImageOutInfo.pDevice = renderEngine->GetDevicePtr();
    asciiImageOutInfo.isSwapChain = false;
    asciiImageOutInfo.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    asciiImageOutInfo.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    renderEngine->StoreFrameBufferPtr("asciiImageOut", FrameBuffer::CreateFrameBuffer(asciiImageOutInfo));
    renderEngine->GetFrameBufferPtr("asciiImageOut")->CreateLayer("rgba", FrameBufferPixelFormat::COLOR_RGBA_8888);
    _outPorts.at("image").SetOutgoingResource(ResourceType::FrameBuffer, "asciiImageOut");

    updateUniformBuffer(renderEngine);
};  

void AsciiImage::BindResources(RenderEngine* renderEngine)
{
    renderEngine->GetFrameBufferPtr("asciiImageOut")->Bind(_pRenderContext, true);

     /* Get Upstream Image via Image Port */
    auto connectedPorts = _inPorts.at("image").GetConnectedPorts();
    for (auto port : connectedPorts)
    {
        if (port->GetIncomingResourceType() == ResourceType::FrameBuffer)
        {
            auto resourceName = port->GetIncomingResourceName();
            renderEngine->GetFrameBufferPtr(resourceName)->Bind(_pRenderContext, false);
        };
    };

    renderEngine->GetBufferPtr("asciiImageSettings")->Bind(_pRenderContext);
    int numberOfChar = 9;
    for (int i = 0; i < numberOfChar; i++)
    {
        std::string filename = "Map_" + std::to_string(i) + ".png";
        renderEngine->GetTexturePtr(filename)->Bind(_pRenderContext);
    };

    renderEngine->GetShaderLibraryPtr()->GetShader("AsciiImage")->Bind(_pRenderContext);
};

void AsciiImage::Execute(RenderEngine* renderEngine)
{
    auto targetWidth = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    auto targetHeight = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    uint32_t workgroupSizeX = ceil(targetWidth / 16);
    uint32_t workgroupSizeY = ceil(targetHeight / 16);
    renderEngine->GetCommandPtr()->DispatchCompute(_pRenderContext, workgroupSizeX, workgroupSizeY, 1);
};

};