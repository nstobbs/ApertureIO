#pragma once

#include "ApertureIO/Knob.hpp"
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
    None = -1,
    Graphics = 1,
    Compute = 2,
    Transfer = 3
};

enum class ResourceType
{
    eNone = -1,
    Vertex = 0,
    Index = 1,
    Uniform = 2,
    Storage = 3,
    Texture = 4,
    FrameBuffer = 5
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
    RenderPass();
    virtual void BuildKnobs() = 0; /* Builds Knobs for Adjusting Parameters of the RenderPass */
    virtual void OnKnobChange(IKnob* knob) = 0; /* Callback Function for Knob Changes */
    virtual void AllocateResources(RenderEngine*  renderEngine) = 0; /* Allocated Required Resources */
    virtual void BindResources(RenderEngine* renderEngine) = 0; /* Bind Resources to the RenderContext */
    virtual void Execute(RenderEngine* renderEngine) = 0; /* Sumbits the Pass for Rendering */
    
    std::vector<ResourceAccess> GetResourcesAccess();
    RenderContext& GetRenderContext();

    KnobManager* GetKnobManger();
    IKnob* GetKnob(const std::string& name);

    /* Connections */
    Port* GetInPort(const std::string& name);
    Port* GetOutPort(const std::string& name);
    std::vector<Port*> GetAllInPorts();
    std::vector<Port*> GetAllOutPorts();

    std::vector<std::string> GetAllInPortNames();
    std::vector<std::string> GetAllOutPortNames();

    std::string GetName();
    uint32_t GetID();

protected:
    /* RenderPass Info  */
    std::string _name;
    uint32_t _ID;
    RenderPassType _type;
    std::vector<ResourceAccess> _resourcesAccess;

    /* Parameters */
    UniquePtr<KnobManager> _knobManager;
    
    /* Connections */
    std::unordered_map<std::string, Port> _inPorts;
    std::unordered_map<std::string, Port> _outPorts;

    /* Rendering Objects */
    Shader* _pShader;
    RenderContext _pRenderContext;
};
};