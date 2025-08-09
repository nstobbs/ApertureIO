#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/Logger.hpp"

#include <set>
#include <stack>


namespace Aio
{

RenderGraph::RenderGraph(Device* pDevice, Context* pContext, FrameBuffer* pTargetSwapChain)
{
    _pDevice = pDevice;
    _pContext = pContext;
    _pTargetSwapChain = pTargetSwapChain;
};

void RenderGraph::AppendRenderPass(RenderPass* pass)
{
    _renderPasses.push_back(pass);
};

void RenderGraph::StoreBufferPtr(std::string name, Buffer* pBuffer)
{
    if (_pBuffersMap.find(name) != _pBuffersMap.end())
    {
        if (_pBuffersMap.at(name) == nullptr)
        {
            _pBuffersMap[name] = pBuffer;
        }
    }
    else
    {
        _pBuffersMap.emplace(name, pBuffer);
    }
};

void RenderGraph::StoreTexturePtr(std::string name, Texture* pTexture)
{
    if (_pTexturesMap.find(name) != _pTexturesMap.end())
    {
        if (_pTexturesMap.at(name) == nullptr)
        {
            _pTexturesMap[name] = pTexture;
        }
    }
    else
    {
        _pTexturesMap.emplace(name, pTexture);
    }
};

Buffer* RenderGraph::GetBufferPtr(std::string name)
{
    auto buffer = _pBuffersMap.at(name);
    if (!buffer)
    {
        auto msg = "Tried Accessing " + name + " but resource handle was a nullptr.";
        Logger::LogError(msg);
    }
    return buffer;
};

Texture* RenderGraph::GetTexturePtr(std::string name)
{
    auto texture = _pTexturesMap.at(name);
    if (!texture)
    {
        auto msg = "Tried Accessing " + name + " but texture handle was a nullptr.";
        Logger::LogError(msg);
    }
    return texture;
};

Context* RenderGraph::GetContextPtr()
{
    return _pContext;
};

Device* RenderGraph::GetDevicePtr()
{
    return _pDevice;
};

FrameBuffer* RenderGraph::GetTargetFrameBufferPtr()
{
    return _pTargetSwapChain;
}

std::vector<RenderPass*> RenderGraph::sortGraphTaskOrder()
{
    std::set<RenderPass*> visited;
    std::stack<RenderPass*> output;

    auto travelGraph = [&](auto&& self, RenderPass* pRenderPass) -> void {
        if (visited.find(pRenderPass) == visited.end())
        {
            visited.emplace(pRenderPass);
        };

        for (auto pass : pRenderPass->GetNextsRenderPasses())
        {
            if (visited.find(pass) == visited.end())
            {
                self(self, pass);
            };
        };

        output.push(pRenderPass);
    };

    for (auto pass : _renderPasses)
    {
        if (visited.find(pass) == visited.end())
        {
            travelGraph(travelGraph, pass);
        }
    };


    std::vector<RenderPass*> result;
    while(!output.empty())
    {
        result.push_back(output.top());
        output.pop();
    };
    return result;
};

void RenderGraph::CompileGraph()
{
    auto taskOrder = sortGraphTaskOrder();
    std::set<RenderPass*> initPasses; /* All the passes that will be allocating resources to the RenderGraph */
    
    /* Compile Requested Resources Vector */
    for(auto pass : taskOrder)
    {
        auto requestedResources = pass->GetResourcesAccess();
        for (auto resource : requestedResources)
        {
            if (resource.type == ResourceType::Texture)
            {
                StoreTexturePtr(resource.name, nullptr);
            }
            else
            {
                StoreBufferPtr(resource.name, nullptr);
            }

            if (resource.isInitialisingResource)
            {
                initPasses.emplace(pass);
            };
        };

        if (initPasses.find(pass) != initPasses.end())
        {
            pass->InitialiseResources(this);
        };

        pass->PreExecutePass(this);
    };
};

void RenderGraph::RenderFrame()
{
    auto taskOrder = sortGraphTaskOrder();
    RenderContext tempContext;
    _pTargetSwapChain->Bind(tempContext);

    GetDevicePtr()->pCommand->BeginFrame(tempContext);
    for (auto pass : taskOrder)
    {
        pass->ExecutePass(this);
    };
    GetDevicePtr()->pCommand->EndFrame(tempContext);
};

};