#pragma once

#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/Shader.hpp"

/* The RenderEngine is that main system for rendering these RenderGraphs.
When LoadGraph is called, RenderEngine will begin allocating needed
resources and prepping the RenderGraphs for execution.

RenderPass = A Task
Port = Connections to other RenderPasses
RenderGraph = Collection of RenderPasses
RenderEngine = Manages Resources and Executes RenderGraphs */

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
    void StoreFrameBufferPtr(std::string name, UniquePtr<FrameBuffer> pFrameBuffer);

    Buffer* GetBufferPtr(std::string name);
    Texture* GetTexturePtr(std::string name);
    FrameBuffer* GetFrameBufferPtr(std::string name);
    
    Context* GetContextPtr();
    Device* GetDevicePtr();
    Command* GetCommandPtr();
    ShaderLibrary* GetShaderLibraryPtr();
    FrameBuffer* GetTargetFrameBufferPtr();

private:

    /* Resources*/
    Device* _pDevice;
    Context* _pContext;
    FrameBuffer* _pTarget;

    UniquePtr<Command> _command;
    UniquePtr<ShaderLibrary> _shaderLibrary;

    std::unordered_map<std::string, UniquePtr<Buffer>> _pBuffersMap;
    std::unordered_map<std::string, UniquePtr<Texture>> _pTexturesMap;
    std::unordered_map<std::string, UniquePtr<FrameBuffer>> _pFrameBufferMap;

    /* Graph */
    RenderGraph* _activeGraph = {nullptr};
    std::unordered_map<std::string, UniquePtr<RenderGraph>> _loadedGraphs;
};

};