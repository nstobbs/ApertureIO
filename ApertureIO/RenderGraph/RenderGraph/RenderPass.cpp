#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

std::vector<ResourceAccess> RenderPass::GetResourcesAccess()
{
    return _resourcesAccess;
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

std::string& RenderPass::GetName()
{
    return _name;
};

};