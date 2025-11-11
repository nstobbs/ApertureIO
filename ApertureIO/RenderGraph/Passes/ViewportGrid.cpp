#include "ApertureIO/ViewportGrid.hpp"

namespace Aio
{

ViewportGrid::ViewportGrid()
{
    /* RenderPass Details */
    _name = "ViewportGrid";
    _type = RenderPassType::Graphics;

    /* Resources Access */
    ResourceAccess vertexBufferAccess{};
    vertexBufferAccess.name = "ViewportGrid_vertexBuffer";
    vertexBufferAccess.type = ResourceType::Vertex;
    vertexBufferAccess.access = AccessType::Read;
    vertexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(vertexBufferAccess);

    ResourceAccess indexBufferAccess{};
    indexBufferAccess.name = "ViewportGrid_indexBuffer";
    indexBufferAccess.type = ResourceType::Index;
    indexBufferAccess.access = AccessType::Read;
    indexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(indexBufferAccess);

    ResourceAccess overlaysFrameBufferAccess{};
    overlaysFrameBufferAccess.name = "Viewport_Overlays";
    overlaysFrameBufferAccess.type = ResourceType::FrameBuffer;
    overlaysFrameBufferAccess.access = AccessType::Both;
    overlaysFrameBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(overlaysFrameBufferAccess);

    /* Ports */
    Port cameraIn(this);
    Port imageOut(this); 

    _inPorts.emplace("camera", cameraIn);
    _outPorts.emplace("image", imageOut);
};

void ViewportGrid::BuildKnobs()
{
    if (auto knob = _knobManager->CreateKnob(KnobType::Int, "GridSize")) {
        _pGridSizeKnob = dynamic_cast<IntKnob*>(knob);
    };
};

void ViewportGrid::AllocateResources(RenderEngine*  renderEngine)
{
    ShaderCreateInfo shaderInfo{};
    shaderInfo.name = "ViewportGrid";
    shaderInfo.pContext = renderEngine->GetContextPtr();
    shaderInfo.pDevice = renderEngine->GetDevicePtr();
    shaderInfo.type = ShaderType::Graphics;
    shaderInfo.sourceFilepath = "./Shaders/ViewportGrid.glsl";
    renderEngine->GetShaderLibraryPtr()->CreateShader(shaderInfo);
    _pShader = renderEngine->GetShaderLibraryPtr()->GetShader("ViewportGrid");

    /* Vertex */
    BufferLayout vertexLayout;
    BufferElement vectorElement;
    vectorElement.count = 3;
    vectorElement.normalized = false;
    vectorElement.type = BufferElementType::Float;
    vertexLayout.AddBufferElement(vectorElement); /* Position */
    vertexLayout.AddBufferElement(vectorElement); /* UV */

    glm::vec3 vertexData[8]; /* Position and UV */
    vertexData[0] = glm::vec3(-1.0f, 1.0f, 0.0f); /* Top Left  0 */
    vertexData[1] = glm::vec3(0.0f, 0.0f, 0.0f); 

    vertexData[2] = glm::vec3(1.0f, 1.0f, 0.0f); /* Top Right  1*/
    vertexData[3] = glm::vec3(1.0f, 0.0f, 0.0f);

    vertexData[4] = glm::vec3(-1.0f, -1.0f, 0.0f); /* Bottom Left 2*/
    vertexData[5] = glm::vec3(0.0f, 1.0f, 0.0f);

    vertexData[6] = glm::vec3(1.0f, -1.0f, 0.0f); /* Bottom Right 3*/
    vertexData[7] = glm::vec3(1.0f, 1.0f, 0.0f);

    BufferCreateInfo vertexInfo{};
    vertexInfo.context = renderEngine->GetContextPtr();
    vertexInfo.device = renderEngine->GetDevicePtr();
    vertexInfo.count = 4;
    vertexInfo.type = BufferType::Vertex;
    vertexInfo.layout = vertexLayout;
    vertexInfo.data = vertexData;
    renderEngine->StoreBufferPtr("ViewportGrid_vertexBuffer", Buffer::CreateBuffer(vertexInfo));

    /* Index */
    BufferLayout indexLayout;
    BufferElement intElement;
    intElement.count = 1;
    intElement.normalized = false;
    intElement.type = BufferElementType::Int;
    indexLayout.AddBufferElement(intElement);

    int indexData[6] = {0,1,3,3,2,0};

    BufferCreateInfo indexInfo{};
    indexInfo.context = renderEngine->GetContextPtr();
    indexInfo.device = renderEngine->GetDevicePtr();
    indexInfo.count = 6;
    indexInfo.type = BufferType::Index;
    indexInfo.layout = indexLayout;
    indexInfo.data = indexData;
    renderEngine->StoreBufferPtr("ViewportGrid_indexBuffer", Buffer::CreateBuffer(indexInfo));

    /* FrameBuffer */
    FrameBufferCreateInfo overlaysInfo{};
    overlaysInfo.name = "Viewport_Overlays";
    overlaysInfo.pContext = renderEngine->GetContextPtr();
    overlaysInfo.pDevice = renderEngine->GetDevicePtr();
    overlaysInfo.isSwapChain = false;
    overlaysInfo.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    overlaysInfo.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    renderEngine->StoreFrameBufferPtr("Viewport_Overlays", FrameBuffer::CreateFrameBuffer(overlaysInfo));
    renderEngine->GetFrameBufferPtr("Viewport_Overlays")->CreateLayer("colour", FrameBufferPixelFormat::COLOR_RGBA_8888);
    renderEngine->GetFrameBufferPtr("Viewport_Overlays")->CreateLayer("depthStencil", FrameBufferPixelFormat::DEPTH_STENCIL_D32_S8);

    /* Pass Viewport Overlays FrameBuffer */
    _outPorts.at("image").SetOutgoingResource(ResourceType::FrameBuffer, "Viewport_Overlays");
};

void ViewportGrid::BindResources(RenderEngine* renderEngine)
{
    auto connectedPorts = _inPorts.at("camera").GetConnectedPorts();
    for (auto port : connectedPorts)
    {
        if (port->GetIncomingResourceType() == ResourceType::Uniform)
        {
            auto cameraBufferName = port->GetIncomingResourceName();
            renderEngine->GetBufferPtr(cameraBufferName)->Bind(_pRenderContext);
        };
    };

    renderEngine->GetFrameBufferPtr("Viewport_Overlays")->Bind(_pRenderContext, true);
    renderEngine->GetBufferPtr("ViewportGrid_vertexBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("ViewportGrid_indexBuffer")->Bind(_pRenderContext);
    renderEngine->GetShaderLibraryPtr()->GetShader("ViewportGrid")->Bind(_pRenderContext);
};

void ViewportGrid::Execute(RenderEngine* renderEngine)
{
    renderEngine->GetCommandPtr()->Draw(_pRenderContext);
};

};