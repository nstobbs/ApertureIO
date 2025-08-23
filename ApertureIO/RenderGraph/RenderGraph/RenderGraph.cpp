#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/Logger.hpp"

#include <set>
#include <stack>


namespace Aio
{

void RenderGraph::AddRenderPass(RenderPass* renderPass)
{
    _renderPasses.push_back(UniquePtr<RenderPass>(renderPass));
};

RenderPass* RenderGraph::CreateRenderPass(const std::string& name)
{
    _renderPasses.push_back(std::move(RenderPassFactory::CreateRenderPass(name)));
    return _renderPasses.back().get();
};

std::vector<RenderPass*> RenderGraph::sortGraph()
{
    std::set<RenderPass*> visited;
    std::stack<RenderPass*> output;

    auto travelGraph = [&](auto&& self, RenderPass* pRenderPass) -> void {
        if (visited.find(pRenderPass) == visited.end())
        {
            visited.emplace(pRenderPass);
        };

        auto outPorts = pRenderPass->GetAllOutPorts();
        for (auto port : outPorts)
        {
            auto passPtr = port->GetRenderPass();
            if (visited.find(passPtr) == visited.end())
            {
                self(self, passPtr);
            };
        };

        output.push(pRenderPass);
    };

    auto passCount = _renderPasses.size();
    for (int i = 0; i < passCount; i++)
    {
        auto passPtr = _renderPasses[i].get();
        if (visited.find(passPtr) == visited.end())
        {
            travelGraph(travelGraph, passPtr);
        };
    };

    std::vector<RenderPass*> result;
    while(!output.empty())
    {
        result.push_back(output.top());
        output.pop();
    };
    return result;
};

void RenderGraph::CompileGraph(RenderEngine* renderEngine)
{
    auto taskOrder = sortGraph();
    std::set<RenderPass*> initPasses; /* All the passes that will be allocating resources to the RenderGraph */
    
    /* Compile Requested Resources Vector */
    for(auto pass : taskOrder)
    {
        auto requestedResources = pass->GetResourcesAccess();
        for (auto resource : requestedResources)
        {
            if (resource.type == ResourceType::Texture)
            {
                renderEngine->StoreTexturePtr(resource.name, nullptr);
            }
            else
            {
                renderEngine->StoreBufferPtr(resource.name, nullptr);
            }

            if (resource.isInitialisingResource)
            {
                initPasses.emplace(pass);
            };
        };

        if (initPasses.find(pass) != initPasses.end())
        {
            pass->AllocateResources(renderEngine);
        };

        pass->BindResources(renderEngine);
    };
};

void RenderGraph::ExecuteGraph(RenderEngine* renderEngine)
{
    auto taskOrder = sortGraph();
    RenderContext rCtx;
    renderEngine->GetTargetFrameBufferPtr()->Bind(rCtx);
    renderEngine->GetCommandPtr()->BeginFrame(rCtx);
    for (auto pass : taskOrder)
    {
        pass->Execute(renderEngine);
    };
    renderEngine->GetCommandPtr()->EndFrame(rCtx);
};

};