#include "ApertureIO/PhongLighting.hpp"
#include "ApertureIO/Logger.hpp"

#include <random>
#include <cmath>

namespace Aio
{

PhongLighting::PhongLighting()
{
    /* RenderPass Info */
    _name = "PhongLighting";
    _type = RenderPassType::Compute;

    /* Resources */
    ResourceAccess lightsBufferAccess{};
    lightsBufferAccess.name = "gLightsBuffer";
    lightsBufferAccess.type = ResourceType::Uniform;
    lightsBufferAccess.access = AccessType::Both;
    lightsBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(lightsBufferAccess);

    ResourceAccess metadataBuffer{};
    metadataBuffer.name = "gMetadataBuffer";
    metadataBuffer.type = ResourceType::Uniform;
    metadataBuffer.access = AccessType::Both;
    metadataBuffer.isInitialisingResource = true;
    _resourcesAccess.push_back(metadataBuffer);

    /* PhongLighting expects an FrameBuffer called gBuffer to be filled with
    the Geo data required to run lighting calculations on.
    Expects:
    gNormals: Geo's Normals
    gColor: Color */
    ResourceAccess gBufferAccess{};
    gBufferAccess.name = "gBuffer";
    gBufferAccess.type = ResourceType::FrameBuffer;
    gBufferAccess.access = AccessType::Read;
    gBufferAccess.isInitialisingResource = false;
    _resourcesAccess.push_back(gBufferAccess);

    /* Image Out FrameBuffer*/
    ResourceAccess imageOutAccess{};
    imageOutAccess.name = "imageOut";
    imageOutAccess.type = ResourceType::FrameBuffer;
    imageOutAccess.access = AccessType::Write;
    imageOutAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(imageOutAccess);

    /* Ports */
    Port imageIn(this);
    Port imageOut(this);
    _inPorts.emplace("image", imageIn);
    _outPorts.emplace("image", imageOut);
};

void PhongLighting::GeneratePointLights(uint32_t numOfLights, float bbox_X, float bbox_Y, float bbox_Z)
{
    if (numOfLights > MAX_LIGHTS)
    {
        auto error = "PhongLighting: Can't generate PointLights if numOfLights are larger than MAX_LIGHTS.";
        Logger::LogError(error);
    };

    _lights.clear();
    _numOfLights = numOfLights;

    std::random_device rngDevice;
    std::mt19937 rng(rngDevice());

    /* Random Position */
    std::uniform_real_distribution<float> dist_X(-bbox_X, bbox_X);
    std::uniform_real_distribution<float> dist_Y(-bbox_Y, bbox_Y);
    std::uniform_real_distribution<float> dist_Z(-bbox_Z, bbox_Z);

    /* Random Color */
    std::uniform_real_distribution<float> dist_Color(0.0f, 1.0f);

    for (int i = 0; i < _numOfLights; i++)
    {
        PointLight light;
        light.intensity = 1.0f;

        light.position.x = dist_X(rng);
        light.position.y = dist_Y(rng);
        light.position.z = dist_Z(rng);

        light.color.x = dist_Color(rng); // R
        light.color.y = dist_Color(rng); // G
        light.color.z = dist_Color(rng); // B

        _lights.push_back(light);
    };
};

void PhongLighting::UpdateLightsBuffer(RenderEngine* renderEngine)
{
    LightsMetadata data{};
    data.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    data.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    data.numOfLights = _numOfLights;
    
    renderEngine->GetBufferPtr("gLightsBuffer")->UploadToDevice(_lights.data());
    renderEngine->GetBufferPtr("gMetadataBuffer")->UploadToDevice(&data);
};

void PhongLighting::AllocateResources(RenderEngine*  renderEngine)
{
    /* Shader Allocation */
    ShaderCreateInfo shaderInfo{};
    shaderInfo.name = "PhongLighting";
    shaderInfo.pContext = renderEngine->GetContextPtr();
    shaderInfo.pDevice = renderEngine->GetDevicePtr();
    shaderInfo.type = ShaderType::Compute;
    shaderInfo.sourceFilepath = "./Shaders/PhongLighting.glsl";
    renderEngine->GetShaderLibraryPtr()->CreateShader(shaderInfo);
    _pShader = renderEngine->GetShaderLibraryPtr()->GetShader("PhongLighting");

    /* Buffer that Stores an Array of PointLights */
    BufferLayout pointLightsLayout;
    BufferElement vectorElement{};
    vectorElement.count = 3;
    vectorElement.type = BufferElementType::Float;
    vectorElement.normalized = false;
    pointLightsLayout.AddBufferElement(vectorElement); /* Postilion Element */
    pointLightsLayout.AddBufferElement(vectorElement); /* Color Element */

    BufferElement intensityElement{};
    intensityElement.count = 1;
    intensityElement.type = BufferElementType::Float;
    intensityElement.normalized = false;
    pointLightsLayout.AddBufferElement(intensityElement);

    BufferCreateInfo gLightsBufferInfo{};
    gLightsBufferInfo.context = renderEngine->GetContextPtr();
    gLightsBufferInfo.device = renderEngine->GetDevicePtr();
    gLightsBufferInfo.type = BufferType::Uniform;
    gLightsBufferInfo.count = MAX_LIGHTS;
    gLightsBufferInfo.layout = pointLightsLayout;
    renderEngine->StoreBufferPtr("gLightsBuffer", Buffer::CreateBuffer(gLightsBufferInfo));

    /* Buffer that store required Metadata */
    BufferLayout metadataLayout;
    BufferElement intElement{};
    intElement.count = 1;
    intElement.type = BufferElementType::Int;
    intElement.normalized = false;
    metadataLayout.AddBufferElement(intElement); /* Target Width */
    metadataLayout.AddBufferElement(intElement); /* Target Height */
    metadataLayout.AddBufferElement(intElement); /* Num of Lights */

    BufferCreateInfo metadataBufferInfo{};
    metadataBufferInfo.context = renderEngine->GetContextPtr();
    metadataBufferInfo.device = renderEngine->GetDevicePtr();
    metadataBufferInfo.type = BufferType::Uniform;
    metadataBufferInfo.count = 1;
    metadataBufferInfo.layout = metadataLayout;
    renderEngine->StoreBufferPtr("gMetadataBuffer", Buffer::CreateBuffer(metadataBufferInfo));

    /* Image Out FrameBuffer */
    FrameBufferCreateInfo imageOutInfo{};
    imageOutInfo.pContext = renderEngine->GetContextPtr();
    imageOutInfo.pDevice = renderEngine->GetDevicePtr();
    imageOutInfo.isSwapChain = false;
    imageOutInfo.name = "imageOut";
    imageOutInfo.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    imageOutInfo.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    renderEngine->StoreFrameBufferPtr("imageOut", FrameBuffer::CreateFrameBuffer(imageOutInfo));
    renderEngine->GetFrameBufferPtr("imageOut")->CreateLayer("rgba", FrameBufferPixelFormat::COLOR_RGBA_8888);

    /* Default Will Create 500 Lights */ 
    GeneratePointLights(500, 1.0f, 1.0f, 1.0f); 
    UpdateLightsBuffer(renderEngine);
};

void PhongLighting::BindResources(RenderEngine* renderEngine)
{
    renderEngine->GetFrameBufferPtr("imageOut")->Bind(_pRenderContext, true);
    renderEngine->GetFrameBufferPtr("gBuffer")->Bind(_pRenderContext, false);
    renderEngine->GetBufferPtr("gLightsBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("gMetadataBuffer")->Bind(_pRenderContext);
    renderEngine->GetShaderLibraryPtr()->GetShader("PhongLighting")->Bind(_pRenderContext);
};

void PhongLighting::Execute(RenderEngine* renderEngine)
{
    auto targetWidth = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    auto targetHeight = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    uint32_t workgroupSizeX = ceil(targetWidth / 16);
    uint32_t workgroupSizeY = ceil(targetHeight / 16);
    renderEngine->GetCommandPtr()->DispatchCompute(_pRenderContext, workgroupSizeX, workgroupSizeY, 1);
};

};