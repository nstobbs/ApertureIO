#include "ApertureIO/RenderGraph.hpp"
#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Knob.hpp"

#include <set>
#include <stack>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>


namespace Aio
{

UniquePtr<RenderGraph> RenderGraph::ReadFromJsonFile(const std::string& filePath)
{
    auto graphPtr = std::make_unique<RenderGraph>();


};

void RenderGraph::WriteToJsonFile(const std::string& filename)
{
    // Header Metadata
    // RenderGraph Name
    QJsonObject rootObj;
    rootObj["RenderGraph_Name"] = "TestGraph";

    /* Pre RenderPass */
    QJsonArray  passesArray;
    auto passes = sortGraph();
    for (auto pass : passes) {
        QJsonObject passObj;
        
        // RenderPass Name & ID
        passObj["Name"] = pass->GetName();
        passObj["ID"] = pass->GetID();

        // RenderPass InPorts
        QJsonArray inPortsArray;
        auto inPorts = pass->GetAllInPortNames();
        for (auto name : inPorts) {
            QJsonObject inPort;
            inPort["Name"] = name;
            inPortsArray.append(inPort);
        };
        passObj["InPorts"] = inPortsArray;

        // RenderPass OutPorts
        QJsonArray outPortArray;
        auto outPorts = pass->GetAllOutPortNames();
        for (auto name : outPorts) {
            QJsonObject outPort;
            outPort["Name"] = name;
            outPortArray.append(outPort);
        };
        passObj["OutPorts"] = outPortArray;

        // RenderPass Knobs
        auto knobs = pass->GetKnobManger()->GetAllKnobs();
        QJsonArray knobArray;
        for (auto knob : knobs) {
            QJsonObject knobObj;
            knobObj["Type"] = knob->GetType();
            knobObj["Name"] = knob->GetName();
            switch(knob->GetType()) {
                case KnobType::Bool:
                    knobObj["Value"] = dynamic_cast<BoolKnob*>(knob)->GetValue();
                    break;

                case KnobType::Int:
                    knobObj["Value"] = dynamic_cast<IntKnob*>(knob)->GetValue();
                    break;
                case KnobType::Float:
                    knobObj["Value"] = dynamic_cast<FloatKnob*>(knob)->GetValue();
                    break;

                case KnobType::String:
                    knobObj["Value"] = dynamic_cast<StringKnob*>(knob)->GetValue().c_str();
                    break;

                case KnobType::Vec2:
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec2Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    knobObj["Value"] = valueObj;
                    break;

                case KnobType::Vec3:
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec3Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    knobObj["Value"] = valueObj;
                    break;

                case KnobType::Vec4:
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec4Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    valueObj["w"] = value.w;
                    knobObj["Value"] = valueObj;
                    break;

                case KnobType::Mat4:
                    Logger::LogWarn("Don't Support Writing Mat4Knobs Yet!");
                    break;
            }
            knobArray.append(knobObj);
        }
        passObj["Knobs"] = knobArray;
        passesArray.append(passObj);
    }
    rootObj["RenderPasses"] = passesArray;

    // Port Connections
    QJsonObject connectionsObj;
    for (auto pass : passes) {
        pass
    }

    // Write to File
    QByteArray byteArray;
    byteArray = QJsonDocument(rootObj).toJson();

    QFile file;
    file.setFileName(filename.c_str());
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::LogWarn("Can't Write to File");
        return;
    }

    file.write(byteArray);
    file.close();
};

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

        // Build RenderPass
        renderPass->BuildKnobs();
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
        renderPass->GetKnobManger()->CheckForKnobChange();
        renderPass->Execute(renderEngine);
        lastRenderPass = renderPass;
    };

    /* TODO: EndFrame needs the last RenderPass to copy that last target framebuffer to copy
    it over to the swapchain image before presenting the frame */
    renderEngine->GetCommandPtr()->EndFrame(lastRenderPass->GetRenderContext() ,rCtx);
};

};