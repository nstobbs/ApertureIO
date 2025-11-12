#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

RenderPass::RenderPass()
{
    _knobManager = std::make_unique<KnobManager>(this);
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

std::string RenderPass::GetName()
{
    return _name;
};

IKnob* RenderPass::GetKnob(const std::string& name)
{
    return _knobManager->GetKnob(name);
};

KnobManager* RenderPass::GetKnobManger()
{
    return _knobManager.get();
};

};