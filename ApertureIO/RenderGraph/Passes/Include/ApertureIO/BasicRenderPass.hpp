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
    BasicRenderPass(WeakPtr<RenderGraph> renderGraph); 
    void InitialiseResources(WeakPtr<RenderGraph> renderGraph) override;
    void PreExecutePass(WeakPtr<RenderGraph> renderGraph) override; 
    void ExecutePass(WeakPtr<RenderGraph> renderGraph) override;

private:
    void updateUniformData(WeakPtr<RenderGraph> renderGraph);
    TestUniformStruct _uniformData;
    std::chrono::system_clock::time_point _startTime;
};
};