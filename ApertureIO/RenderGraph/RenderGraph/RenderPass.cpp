#include "ApertureIO/RenderPass.hpp"

//FIXME: Need to Find a Unique way of Creating this
// ID number without creating an Pass with the Same
// Name and ID number.
namespace {
    uint32_t globalRenderPassCount = {0};
}

namespace Aio
{

RenderPass::RenderPass()
{
    _knobManager = std::make_unique<KnobManager>(this);
    _ID = globalRenderPassCount++;
};

std::vector<ResourceAccess> RenderPass::GetResourcesAccess()
{
    return _resourcesAccess;
};

RenderContext& RenderPass::GetRenderContext()
{
    return _pRenderContext;
};

Port* RenderPass::GetInPort(const std::string& name)
{
    return &_inPorts.at(name);
};

Port* RenderPass::GetOutPort(const std::string& name)
{
    return &_outPorts.at(name);
};

std::vector<Port*> RenderPass::GetAllInPorts()
{
    std::vector<Port*> ports;
    for (auto& port : _inPorts)
    {
        ports.push_back(&port.second);
    };

    return ports;
};

std::vector<Port*> RenderPass::GetAllOutPorts()
{
    std::vector<Port*> ports;
    for (auto& port : _outPorts)
    {
        ports.push_back(&port.second);
    };

    return ports; 
};

std::vector<std::string> RenderPass::GetAllInPortNames()
{
    std::vector<std::string> names;
    for (auto port : _inPorts) {
        names.push_back(port.first);
    }
    return names;
};

std::vector<std::string> RenderPass::GetAllOutPortNames()
{
    std::vector<std::string> names;
    for (auto port : _outPorts) {
        names.push_back(port.first);
    }
    return names;
};

std::string RenderPass::GetName()
{
    return _name;
};

uint32_t RenderPass::GetID()
{
    return _ID;
};

void RenderPass::SetID(uint32_t id)
{
    _ID = id;
};

std::string RenderPass::GetUniqueName()
{
    /* Name + ID = UniqueName*/
    return std::string(GetName() + std::to_string(GetID()));
}; 

KnobGeneric* RenderPass::GetKnob(const std::string& name)
{
    return _knobManager->GetKnob(name);
};

KnobManager* RenderPass::GetKnobManger()
{
    return _knobManager.get();
};

};