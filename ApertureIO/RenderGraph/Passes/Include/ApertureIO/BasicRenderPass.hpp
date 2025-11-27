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
    BasicRenderPass();
    void BuildKnobs() override;
    void OnKnobChange(KnobGeneric* knob) override;
    void AllocateResources(RenderEngine* renderEngine) override;
    void BindResources(RenderEngine* renderEngine) override; 
    void Execute(RenderEngine* renderEngine) override;

private:
    void updateUniformData(RenderEngine* renderEngine);
    TestUniformStruct _uniformData;
    std::chrono::system_clock::time_point _startTime;
};
};