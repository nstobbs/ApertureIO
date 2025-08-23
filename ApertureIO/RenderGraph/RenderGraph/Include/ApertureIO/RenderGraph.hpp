#pragma once
#include "ApertureIO/RenderEngine.hpp"
#include "ApertureIO/RenderPass.hpp"
#include "ApertureIO/RenderPassFactory.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIo/Buffers.hpp"
#include "ApertureIO/FrameBuffer.hpp"

#include <string>
#include <unordered_map>

namespace Aio
{

class RenderPass;
class RenderEngine;

class RenderGraph
{
public:
    void AddRenderPass(RenderPass* renderPass);
    RenderPass* CreateRenderPass(const std::string& name);
    //void RemoveRenderPass(RenderPass* renderPass);
    
    void CompileGraph(RenderEngine* renderEngine); /* Preps the Graph for Rendering, allocate required resources. */
    void ExecuteGraph(RenderEngine* renderEngine); /* Renders Current Frame*/

private:
    std::vector<RenderPass*> sortGraph(); /* Does Topological Sort on the DAG */
    std::vector<UniquePtr<RenderPass>> _renderPasses;
};

};