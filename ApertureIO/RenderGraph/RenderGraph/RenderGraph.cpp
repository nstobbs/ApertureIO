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
            auto connectedPorts = port->GetConnectedPorts();
            for (auto inPort : connectedPorts)
            {
                auto renderPass = inPort->GetRenderPass();
                if (visited.find(renderPass) == visited.end())
                {
                    self(self, renderPass);
                };
            }
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
    for(auto renderPass : taskOrder)
    {
        auto requestedResources = renderPass->GetResourcesAccess();
        for (auto resource : requestedResources)
        {
            if (resource.type == ResourceType::Texture)
            {
                renderEngine->StoreTexturePtr(resource.name, nullptr);
            }
            else if (resource.type == ResourceType::Uniform || resource.type == ResourceType::Vertex || resource.type == ResourceType::Index)
            {
                renderEngine->StoreBufferPtr(resource.name, nullptr);
            }
            else if (resource.type == ResourceType::FrameBuffer)
            {
                renderEngine->StoreFrameBufferPtr(resource.name, nullptr);
            }

            if (resource.isInitialisingResource)
            {
                initPasses.emplace(renderPass);
            };
        };

        if (initPasses.find(renderPass) != initPasses.end())
        {
            renderPass->AllocateResources(renderEngine);
        };

        renderPass->BindResources(renderEngine);
    };
};

void RenderGraph::ExecuteGraph(RenderEngine* renderEngine)
{
    auto taskOrder = sortGraph();
    RenderContext rCtx;

    renderEngine->GetTargetFrameBufferPtr()->Bind(rCtx, true);
    renderEngine->GetCommandPtr()->BeginFrame(rCtx);
    RenderPass* lastRenderPass = nullptr;

    for (auto renderPass : taskOrder)
    {
        renderPass->Execute(renderEngine);
        lastRenderPass = renderPass;
    };

    /* TODO: EndFrame needs the last RenderPass to copy that last target framebuffer to copy
    it over to the swapchain image before presenting the frame */
    renderEngine->GetCommandPtr()->EndFrame(lastRenderPass->GetRenderContext() ,rCtx);
};

};