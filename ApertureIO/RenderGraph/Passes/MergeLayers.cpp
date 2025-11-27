#include "ApertureIO/MergeLayers.hpp"

namespace Aio
{

MergeLayers::MergeLayers()
{
    /* RenderPass Details */
    _name = "MergeLayers";
    _type = RenderPassType::Compute;

    /* Resources Access */
    ResourceAccess outputAccess{};
    outputAccess.name = "MergeLayers_Output";
    outputAccess.type = ResourceType::FrameBuffer;
    outputAccess.access = AccessType::Write;
    outputAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(outputAccess);

    /* Ports */
    Port imageIn_A(this);
    Port imageIn_B(this);

    Port imageOut(this); 

    _inPorts.emplace("imageA", imageIn_A);
    _inPorts.emplace("imageB", imageIn_B);
    
    _outPorts.emplace("image", imageOut);
};

void MergeLayers::BuildKnobs()
{

};

void MergeLayers::OnKnobChange(KnobGeneric* knob)
{
    
};

void MergeLayers::AllocateResources(RenderEngine*  renderEngine)
{
    ShaderCreateInfo shaderInfo{};
    shaderInfo.name = "MergeLayers";
    shaderInfo.pContext = renderEngine->GetContextPtr();
    shaderInfo.pDevice = renderEngine->GetDevicePtr();
    shaderInfo.type = ShaderType::Compute;
    shaderInfo.sourceFilepath = "./Shaders/MergeLayers.glsl";
    renderEngine->GetShaderLibraryPtr()->CreateShader(shaderInfo);
    _pShader = renderEngine->GetShaderLibraryPtr()->GetShader("MergeLayers");

    /* FrameBuffer */
    FrameBufferCreateInfo outputInfo{};
    outputInfo.name = "MergeLayers_Output";
    outputInfo.pContext = renderEngine->GetContextPtr();
    outputInfo.pDevice = renderEngine->GetDevicePtr();
    outputInfo.isSwapChain = false;
    outputInfo.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    outputInfo.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    renderEngine->StoreFrameBufferPtr("MergeLayers_Output", FrameBuffer::CreateFrameBuffer(outputInfo));
    renderEngine->GetFrameBufferPtr("MergeLayers_Output")->CreateLayer("colour", FrameBufferPixelFormat::COLOR_RGBA_8888);
    renderEngine->GetFrameBufferPtr("MergeLayers_Output")->CreateLayer("depthStencil", FrameBufferPixelFormat::DEPTH_STENCIL_D32_S8);

    /* Pass Viewport Overlays FrameBuffer */
    _outPorts.at("image").SetOutgoingResource(ResourceType::FrameBuffer, "MergeLayers_Output");
};

void MergeLayers::BindResources(RenderEngine* renderEngine)
{
    auto connectedPorts_A = _inPorts.at("imageA").GetConnectedPorts();
    auto connectedPorts_B = _inPorts.at("imageB").GetConnectedPorts();
    for (auto port : connectedPorts_A)
    {
        if (port->GetIncomingResourceType() == ResourceType::FrameBuffer)
        {
            auto frameBufferName = port->GetIncomingResourceName();
            renderEngine->GetFrameBufferPtr(frameBufferName)->Bind(_pRenderContext, false);
        };
    };

    for (auto port : connectedPorts_B)
    {
        if (port->GetIncomingResourceType() == ResourceType::FrameBuffer)
        {
            auto frameBufferName = port->GetIncomingResourceName();
            renderEngine->GetFrameBufferPtr(frameBufferName)->Bind(_pRenderContext, false);
        };
    };

    renderEngine->GetFrameBufferPtr("MergeLayers_Output")->Bind(_pRenderContext, true);
    renderEngine->GetShaderLibraryPtr()->GetShader("MergeLayers")->Bind(_pRenderContext);
};

void MergeLayers::Execute(RenderEngine* renderEngine)
{
    auto targetWidth = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    auto targetHeight = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    uint32_t workgroupSizeX = ceil(targetWidth / 16);
    uint32_t workgroupSizeY = ceil(targetHeight / 16);
    renderEngine->GetCommandPtr()->DispatchCompute(_pRenderContext, workgroupSizeX, workgroupSizeY, 1);
};

};