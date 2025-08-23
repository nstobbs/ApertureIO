#include "ApertureIO/Port.hpp"

namespace Aio
{

Port::Port(RenderPass* renderPass)
{
    _pass = renderPass;
};

void Port::Connect(Port* otherPort)
{
    _PortConnections.push_back(otherPort);
    otherPort->GetConnectedPorts().push_back(this);
};

void Port::Disconnect(Port* otherPort)
{
    for (auto portPtr : _PortConnections)
    {
        if(portPtr == otherPort)
        {
            //TODO: This is dumb but whatever for now I guess..
            auto otherPortConnectedPorts = otherPort->GetConnectedPorts();
            _PortConnections.erase(std::remove(_PortConnections.begin(), _PortConnections.end(), portPtr), _PortConnections.end());
            otherPortConnectedPorts.erase(std::remove(otherPortConnectedPorts.begin(), otherPortConnectedPorts.end(), this), otherPortConnectedPorts.end());
            break;
        }
    }
};

void Port::DisconnectAll()
{
    for (auto portPtr : _PortConnections)
    {
        auto ports = portPtr->GetConnectedPorts();
        ports.erase(std::remove(ports.begin(), ports.end(), this), ports.end());
        _PortConnections.erase(std::remove(_PortConnections.begin(), _PortConnections.end(), portPtr), _PortConnections.end());
    }
};

RenderPass* Port::GetRenderPass()
{
    return _pass;
};

std::vector<Port*>& Port::GetConnectedPorts()
{
    return _PortConnections; 
}

};