#pragma once

#include "ApertureIO/RenderContext.hpp
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Handles.hpp
#include "ApertureIo/Buffers.hpp"

#include <string>
#include <vector>

namespace Aio
{
enum class RenderPassType
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2
};

enum class ResourceType
{
    eNone = -1,
    Vertex = 0,
    Index = 1,
    Uniform = 2,
    Storage = 3,
    Texture = 4,
};

enum class AccessType {
    Read = 0,
    Write = 1,
    Both = 2
};

struct ResourceAccess
{
    std::string name;
    ResourceType type;
    AccessType access;
    bool isInitialisingResource; 
};

class RenderGraph; // Forward Declarations 

class RenderPass
{
public:
    virtual RenderPass(RenderGraph* renderGraph) = 0; /* Constructors */
    virtual void InitialiseResources(RenderGraph* renderGraph) = 0; /* Allocated Required Resources */
    virtual void PreExecutePass(RenderGraph* renderGraph) = 0; /* Runs before the start of the MainRenderLoop. For Binding to RenderContext */
    virtual void ExecutePass(RenderGraph* renderGraph) = 0; /* Sumbits the Pass for Rendering */
    

    void AppendRenderPass(RenderPass* pRenderPass);
    std::vector<ResourceAccess> GetResourcesAccess();

protected:
    std::string _name;
    RenderPassType _type;
    std::vector<ResourceAccess> _resourcesAccess;
    std::vector<RenderPass*> _nextsPasses;

/* Rendering Objects */
    Shader* _pShader = {nullptr};
    RenderContext* _pRenderContext = {nullptr};
};
};