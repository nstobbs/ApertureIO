#pragma once

#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/Device.hpp"

/* The RenderEngine is that main system for rendering these Graphs.
When LoadGraph is called, RenderEngine will begin allocating needed
resources and prepping the graph for execution. Once the  

RenderPass = A Tasks
Port = Connections to other RenderPasses
RenderGraph = Collection of Tasks
RenderEngine = Manages Resources and Executes Graphs*/

namespace Aio
{

class RenderGraph;

class RenderEngine
{
public:
    //~RenderEngine();
    RenderEngine(Device* pDevice, Context* pContext, FrameBuffer* pTarget);

    void ExecuteFrame();

    void SetActive(const std::string& name);
    void LoadGraph(const std::string& name, UniquePtr<RenderGraph> renderGraph);
    void RemoveGraph(const std::string& name);

    void StoreBufferPtr(std::string name, UniquePtr<Buffer>pBuffer);
    void StoreTexturePtr(std::string name, UniquePtr<Texture> pTexture);

    Buffer* GetBufferPtr(std::string name);
    Texture* GetTexturePtr(std::string name);
    Context* GetContextPtr();
    Device* GetDevicePtr();
    Command* GetCommandPtr();
    FrameBuffer* GetTargetFrameBufferPtr();

private:

    /* Resources*/
    Device* _pDevice;
    Context* _pContext;
    FrameBuffer* _pTarget;

    UniquePtr<Command> _command;

    std::unordered_map<std::string, UniquePtr<Buffer>> _pBuffersMap;
    std::unordered_map<std::string, UniquePtr<Texture>> _pTexturesMap;

    /* Graph */
    RenderGraph* _activeGraph = {nullptr};
    std::unordered_map<std::string, UniquePtr<RenderGraph>> _loadedGraphs;
};

};