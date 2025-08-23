#pragma once

#include "ApertureIO/Port.hpp"
#include "ApertureIO/RenderEngine.hpp"

#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Handles.hpp"
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

class RenderEngine;
class Port;

class RenderPass
{
public:
    virtual void AllocateResources(RenderEngine*  renderEngine) = 0; /* Allocated Required Resources */
    virtual void BindResources(RenderEngine* renderEngine) = 0; /* Bind Resources to the RenderContext */
    virtual void Execute(RenderEngine* renderEngine) = 0; /* Sumbits the Pass for Rendering */
    
    std::vector<ResourceAccess> GetResourcesAccess();

    /* Connections */
    Port* GetInPort(const std::string& name);
    Port* GetOutPort(const std::string& name);
    std::vector<Port*> GetAllInPorts();
    std::vector<Port*> GetAllOutPorts();

    std::string& GetName(); 

protected:
    /* RenderPass Info  */
    std::string _name;
    RenderPassType _type;
    std::vector<ResourceAccess> _resourcesAccess;
    
    /* Connections */
    std::unordered_map<std::string, Port> _inPorts;
    std::unordered_map<std::string, Port> _outPorts;

    /* Rendering Objects */
    UniquePtr<Shader> _pShader;
    RenderContext _pRenderContext;
};
};