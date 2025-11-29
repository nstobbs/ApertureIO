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
    QFile file;
    file.setFileName(filePath.c_str());
    if(!file.open(QIODevice::ReadOnly)) {
        auto err = "RenderGraph: Failed to Read Json File - " + filePath;
        Logger::LogError(err);
    }

    QByteArray byteArray;
    byteArray = file.readAll();
    file.close();

    //FIXME: this could be written better...
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        auto err = "RenderGraph: Error while Parsing :" + 
        std::to_string(parseError.offset) + ":" + parseError.errorString().toStdString();
        Logger::LogError(err);
    };

    QJsonObject rootObj = jsonDoc.object();

    std::string renderGraphName = rootObj.value("RenderGraphName").toString().toStdString();
    QJsonArray passesArray = rootObj["RenderPasses"].toArray();
    QJsonArray connectionsArray = rootObj["RenderPassConnections"].toArray();

    auto graph = std::make_unique<RenderGraph>();
    std::unordered_map<std::string, RenderPass*> createdPasses;

    for (auto pass : passesArray) {
        /* Create RenderPass */
        QJsonObject passObj = pass.toObject();
        auto passName = passObj.value("RenderPassName").toString().toStdString();
        auto passID = std::stoi(passObj.value("RenderPassID").toString().toStdString());
        auto currentPass = graph->CreateRenderPass(passName);
        currentPass->SetID(passID);
        createdPasses.emplace(currentPass->GetUniqueName(), currentPass);

        /* Set Knobs */
        QJsonArray knobArray = passObj.value("RenderPassKnobs").toArray();
        for (auto knob : knobArray) {
            QJsonObject knobObj = knob.toObject();
            auto knobName = knobObj.value("KnobName").toString().toStdString();
            auto knobType = to_KnobType(knobObj.value("KnobType").toString().toStdString());
            auto currentKnob = currentPass->GetKnob(knobName);

            switch (knobType) {
                case KnobType::Bool: {
                    bool value = knobObj.value("KnobValue").toBool();
                    std::get_if<BoolKnob>(currentKnob)->SetValue(value);
                    break;
                }
                
                case KnobType::Float: {
                    float value = static_cast<float>(knobObj.value("KnobValue").toDouble());
                    std::get_if<FloatKnob>(currentKnob)->SetValue(value);
                    break;
                }

                case KnobType::Int: {
                    int value = knobObj.value("KnobValue").toInt();
                    std::get_if<IntKnob>(currentKnob)->SetValue(value);
                    break;
                }

                case KnobType::String: {
                    QString value = knobObj.value("KnobValue").toString();
                    std::get_if<StringKnob>(currentKnob)->SetValue(value.toStdString());
                    break;
                }
                
                case KnobType::Vec2: {
                    QJsonObject valueObj = knobObj.value("KnobValue").toObject();
                    glm::vec2 value = glm::vec2(valueObj.value("x").toDouble(),
                                                valueObj.value("y").toDouble());
                    std::get_if<Vec2Knob>(currentKnob)->SetValue(value);
                }

                case KnobType::Vec3: {
                    QJsonObject valueObj = knobObj.value("KnobValue").toObject();
                    glm::vec3 value;
                    value.x = valueObj.value("x").toDouble();
                    value.y = valueObj.value("y").toDouble();
                    value.z = valueObj.value("z").toDouble();
                    std::get_if<Vec3Knob>(currentKnob)->SetValue(value);
                }

                case KnobType::Vec4: {
                    QJsonObject valueObj = knobObj.value("KnobValue").toObject();
                    glm::vec4 value;
                    value.x = valueObj.value("x").toDouble();
                    value.y = valueObj.value("y").toDouble();
                    value.z = valueObj.value("z").toDouble();
                    value.w = valueObj.value("w").toDouble();
                    std::get_if<Vec4Knob>(currentKnob)->SetValue(value);
                }

                case KnobType::Mat4: {
                    Logger::LogWarn("Don't Support Reading Mat4Knobs Yet!");
                    break;
                }
            }
        }
    }

    /* Connect Ports */
    /* Go over all of the OutPorts and connect them 
    to the InPort. */
    /* This Part of the File Reading is gonna be slow as hell 
    and really needs rethinking. As this won't scale well at all.*/
    for (auto connection : connectionsArray) {
        QJsonObject connectionObj = connection.toObject();
        QJsonObject outPortsObj = connectionObj.value("outPorts").toObject();
        auto passUniqueName = connectionObj.value("RenderPassUniqueID").toString().toStdString();
        auto currentPass = createdPasses.at(passUniqueName);
        auto outPortNames = currentPass->GetAllOutPortNames();

        for (auto portName : outPortNames) {
            if (outPortsObj.find(portName.c_str()) != outPortsObj.end()) {
                auto incomingPassUniqueID = outPortsObj.value(portName.c_str()).toString().toStdString();
                auto incomingPass = createdPasses.at(incomingPassUniqueID);
                currentPass->GetOutPort(portName)->Connect(incomingPass->GetInPort(portName));
            }
        }
    }

    return graph;
};

void RenderGraph::WriteToJsonFile(const std::string& filename)
{
    // Header Metadata
    // RenderGraph Name
    QJsonObject rootObj;

    /* Get the Filename without File Extension from FilePath */
    std::string baseFilenameWithExt = filename.substr(filename.find_last_of("/\\") + 1);
    std::string::size_type const lastDot(baseFilenameWithExt.find_last_of('.'));
    std::string baseFilename = baseFilenameWithExt.substr(0, lastDot);

    rootObj["RenderGraphName"] = baseFilename.c_str();
    
    /* Per RenderPass */
    QJsonArray  passesArray;
    auto passes = sortGraph();
    for (auto pass : passes) {
        QJsonObject passObj;

        // RenderPass Name & ID
        passObj["RenderPassName"] = pass->GetName().c_str();
        passObj["RenderPassID"] = std::to_string(pass->GetID()).c_str();

        /* Don't think we need to save what all of the ports are for each pass.
        Since these will be created by the Default Constructor of each pass. We just 
        need to know what they are connected to in the Connection List.*/
        // RenderPass InPorts
        //QJsonArray inPortsArray;
        //auto inPorts = pass->GetAllInPortNames();
        //for (auto name : inPorts) {
        //    QJsonObject inPort;
        //    inPort["InPortName"] = name.c_str();
        //    inPortsArray.append(inPort);
        //};
        //passObj["RenderPassInPorts"] = inPortsArray;

        // RenderPass OutPorts
        //QJsonArray outPortArray;
        //auto outPorts = pass->GetAllOutPortNames();
        //for (auto name : outPorts) {
        //    QJsonObject outPort;
        //    outPort["OutPortName"] = name.c_str();
        //    outPortArray.append(outPort);
        //};
        //passObj["RenderPassOutPorts"] = outPortArray;

        // RenderPass Knobs
        auto knobs = pass->GetKnobManger()->GetAllKnobs();
        QJsonArray knobArray;
        for (auto knob : knobs) {
            QJsonObject knobObj;
            // Get Knob Info
            std::string knobName = std::visit([](auto&& thisKnob){
                return thisKnob.GetName();
            }, *knob);
            KnobType knobType = std::visit([](auto&& thisKnob){
                return thisKnob.GetType();
            }, *knob);

            knobObj["KnobType"] = to_string(knobType).c_str();
            knobObj["KnobName"] = knobName.c_str();
            switch(knobType) {
                case KnobType::Bool: {
                    knobObj["KnobValue"] = std::get<BoolKnob>(*knob).GetValue();
                    break;
                }
                case KnobType::Int: {
                    knobObj["KnobValue"] = std::get<IntKnob>(*knob).GetValue();
                    break;
                }
                case KnobType::Float: {
                    knobObj["KnobValue"] = std::get<FloatKnob>(*knob).GetValue();
                    break;
                }
                case KnobType::String: {
                    knobObj["KnobValue"] = std::get<StringKnob>(*knob).GetValue().c_str();
                    break;
                }
                case KnobType::Vec2: {
                    QJsonObject valueObj;
                    auto value = std::get<Vec2Knob>(*knob).GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    knobObj["KnobValue"] = valueObj;
                    break;
                }
                case KnobType::Vec3: {
                    QJsonObject valueObj;
                    auto value = std::get<Vec3Knob>(*knob).GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    knobObj["KnobValue"] = valueObj;
                    break;
                }
                case KnobType::Vec4: {
                    QJsonObject valueObj;
                    auto value = std::get<Vec4Knob>(*knob).GetValue();
                    valueObj["x"] = value.x;
                    valueObj["y"] = value.y;
                    valueObj["z"] = value.z;
                    valueObj["w"] = value.w;
                    knobObj["KnobValue"] = valueObj;
                    break;
                }
                case KnobType::Mat4: {
                    Logger::LogWarn("Don't Support Writing Mat4Knobs Yet!");
                    break;
                }
            }
            knobArray.append(knobObj);
        }
        passObj["RenderPassKnobs"] = knobArray;
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

        /* RenderPassName + ID = RenderPassUnique */
        std::string CurrentRenderPassUnique = pass->GetName() + std::to_string(pass->GetID());
        passObj["RenderPassUniqueID"] = CurrentRenderPassUnique.c_str();

        for (auto portName : inPortNames) {
            if (auto port = pass->GetInPort(portName)) {
                auto incomingPorts = port->GetConnectedPorts();
                for (auto incomingPort : incomingPorts) {
                    if (auto connectedPass = incomingPort->GetRenderPass()) {
                        std::string RenderPassUnique = connectedPass->GetName() + std::to_string(connectedPass->GetID());
                        inPortsObj.insert(portName.c_str(), RenderPassUnique.c_str());
                    }
                }
            }
        }

        for (auto portName : outPortNames) {
            if (auto port = pass->GetOutPort(portName)) {
                auto incomingPorts = port->GetConnectedPorts();
                for (auto incomingPort : incomingPorts) {
                    if (auto connectedPass = incomingPort->GetRenderPass()) {
                        /* RenderPassName + ID = RenderPassUnique */
                        std::string RenderPassUnique = connectedPass->GetName() + std::to_string(connectedPass->GetID());
                        outPortsObj.insert(portName.c_str(), RenderPassUnique.c_str());
                    }
                }
            }
        }
        passObj["inPorts"] = inPortsObj;
        passObj["outPorts"] = outPortsObj;
        connectionsArray.append(passObj);
    }
    rootObj["RenderPassConnections"] = connectionsArray;

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

RenderPass* RenderGraph::FindRenderPass(const std::string& UniqueID)
{
    for (auto& pass :  _renderPasses) {
        auto uniqueName = pass->GetUniqueName();
        if (uniqueName == UniqueID) {
            return pass.get();
        }
    }
    return nullptr;
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
        renderPass->GetKnobManger()->CheckForKnobChange();
        renderPass->Execute(renderEngine);
        lastRenderPass = renderPass;
    };

    /* TODO: EndFrame needs the last RenderPass to copy that last target framebuffer to copy
    it over to the swapchain image before presenting the frame */
    renderEngine->GetCommandPtr()->EndFrame(lastRenderPass->GetRenderContext() ,rCtx);
};

};