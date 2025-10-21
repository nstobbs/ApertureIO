#pragma once

#include <vector>
#include <string>

namespace Aio
{

class RenderPass;
enum class ResourceType;

class Port
{
public:
    Port(RenderPass* renderPass);
    void Connect(Port* otherPort);
    void Disconnect(Port* otherPort);
    void DisconnectAll();

    RenderPass* GetRenderPass();

    /* Transferable Resource */
    std::string GetIncomingResourceName();
    ResourceType GetIncomingResourceType();
    void SetOutgoingResource(ResourceType type, std::string name);

    std::vector<Port*>& GetConnectedPorts();
    
private:
    std::vector<Port*> _PortConnections;
    RenderPass* _pass;

    /* Transferable Data*/
    ResourceType _resourceType;
    std::string _resourceName;
};

};