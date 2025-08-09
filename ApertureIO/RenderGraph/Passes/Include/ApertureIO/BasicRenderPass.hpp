#pragma once

#include "ApertureIO/RenderPass.hpp"
#include "ApertureIO/RenderGraph.hpp"

const float PI = 3.14f;

/* Use basic test RenderPass. Render an Texture onScreen with an uniform data sending data
from the CPU to the GPU to be used within the Shader.*/

namespace Aio
{

struct TestUniformStruct
{
    float a;
};

class BasicRenderPass : public RenderPass
{
public:
    BasicRenderPass(RenderGraph* renderGraph) override; 
    void InitialiseResources(RenderGraph* renderGraph) override;
    void PreExecutePass(RenderGraph* renderGraph) override; 
    void ExecutePass(RenderGraph* renderGraph) override;

private:
    void updateUniformData();
    TestUniformStruct _uniformData;
};
};