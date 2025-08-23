#pragma once

#include "ApertureIO/RenderPass.hpp"
#include <vector>

namespace Aio
{

class RenderPass;

class Port
{
public:
    Port(RenderPass* renderPass);
    void Connect(Port* otherPort);
    void Disconnect(Port* otherPort);
    void DisconnectAll();

    RenderPass* GetRenderPass();

    std::vector<Port*>& GetConnectedPorts();
    
private:
    std::vector<Port*> _PortConnections;
    RenderPass* _pass;
};

};