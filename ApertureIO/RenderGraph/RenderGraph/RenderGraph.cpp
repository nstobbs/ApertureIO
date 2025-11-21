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
    return graphPtr;
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
        passObj["Name"] = pass->GetName().c_str();
        passObj["ID"] = std::to_string(pass->GetID()).c_str();

        // RenderPass InPorts
        QJsonArray inPortsArray;
        auto inPorts = pass->GetAllInPortNames();
        for (auto name : inPorts) {
            QJsonObject inPort;
            inPort["Name"] = name.c_str();
            inPortsArray.append(inPort);
        };
        passObj["InPorts"] = inPortsArray;

        // RenderPass OutPorts
        QJsonArray outPortArray;
        auto outPorts = pass->GetAllOutPortNames();
        for (auto name : outPorts) {
            QJsonObject outPort;
            outPort["Name"] = name.c_str();
            outPortArray.append(outPort);
        };
        passObj["OutPorts"] = outPortArray;

        // RenderPass Knobs
        auto knobs = pass->GetKnobManger()->GetAllKnobs();
        QJsonArray knobArray;
        for (auto knob : knobs) {
            QJsonObject knobObj;
            knobObj["Type"] = to_string(knob->GetType()).c_str();
            knobObj["Name"] = knob->GetName().c_str();
            switch(knob->GetType()) {
                case KnobType::Bool: {
                    knobObj["Value"] = dynamic_cast<BoolKnob*>(knob)->GetValue();
                }
                case KnobType::Int: {
                    knobObj["Value"] = dynamic_cast<IntKnob*>(knob)->GetValue();
                }
                case KnobType::Float: {
                    knobObj["Value"] = dynamic_cast<FloatKnob*>(knob)->GetValue();
                }
                case KnobType::String: {
                    knobObj["Value"] = dynamic_cast<StringKnob*>(knob)->GetValue().c_str();
                }
                case KnobType::Vec2: {
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec2Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    knobObj["Value"] = valueObj;
                }
                case KnobType::Vec3: {
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec3Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    knobObj["Value"] = valueObj;
                }
                case KnobType::Vec4: {
                    QJsonObject valueObj;
                    auto value = dynamic_cast<Vec4Knob*>(knob)->GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    valueObj["w"] = value.w;
                    knobObj["Value"] = valueObj;
                }
                case KnobType::Mat4: {
                    Logger::LogWarn("Don't Support Writing Mat4Knobs Yet!");
                }
            }
            knobArray.append(knobObj);
        }
        passObj["Knobs"] = knobArray;
        passesArray.append(passObj);
    }
    rootObj["RenderPasses"] = passesArray;

    // Port Connections
    QJsonArray connectionsArray;
    for (auto pass : passes) {
        QJsonObject passObj;
        QJsonObject inPortsObj;
        QJsonObject outPortsObj;
        auto inPortNames = pass->GetAllInPortNames();
        auto outPortNames = pass->GetAllOutPortNames();

        for (auto portName : inPortNames) {
            if (auto port = pass->GetInPort(portName)) {
                auto incomingPorts = port->GetConnectedPorts();
                for (auto incomingPort : incomingPorts) {
                    if (auto connectedPass = incomingPort->GetRenderPass()) {
                        inPortsObj.insert(portName.c_str(), std::to_string(connectedPass->GetID()).c_str());
                    }
                }
            }
        }

        for (auto portName : outPortNames) {
            if (auto port = pass->GetOutPort(portName)) {
                auto incomingPorts = port->GetConnectedPorts();
                for (auto incomingPort : incomingPorts) {
                    if (auto connectedPass = incomingPort->GetRenderPass()) {
                        outPortsObj.insert(portName.c_str(), std::to_string(connectedPass->GetID()).c_str());
                    }
                }
            }
        }
        passObj["inPorts"] = inPortsObj;
        passObj["outPorts"] = inPortsObj;
        connectionsArray.append(passObj);
    }
    rootObj["Connections"] = connectionsArray;

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