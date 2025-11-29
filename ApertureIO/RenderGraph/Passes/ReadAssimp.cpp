#include <ApertureIO/ReadAssimp.hpp>
#include <chrono>

namespace {
    const std::string kModelFilePathKnobName = "Model_FilePath";
    const std::string kTextureFilePathKnobName = "Texture_FilePath";
}

namespace Aio
{

ReadAssimp::ReadAssimp()
{
    /* RenderPass Details */
    _name = "ReadAssimp";
    _type = RenderPassType::Graphics;

    /* Resources Access */
    ResourceAccess vertexBufferAccess{};
    vertexBufferAccess.name = "ReadAssimp_vertexBuffer";
    vertexBufferAccess.type = ResourceType::Vertex;
    vertexBufferAccess.access = AccessType::Read;
    vertexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(vertexBufferAccess);

    ResourceAccess indexBufferAccess{};
    indexBufferAccess.name = "ReadAssimp_indexBuffer";
    indexBufferAccess.type = ResourceType::Index;
    indexBufferAccess.access = AccessType::Read;
    indexBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(indexBufferAccess);

    ResourceAccess transformBufferAccess{
        .name = {"ReadAssimp_transformsBuffer"},
        .type = ResourceType::Uniform,
        .access = AccessType::Both,
        .isInitialisingResource = true};
    _resourcesAccess.push_back(transformBufferAccess);
    
    ResourceAccess textureBufferAccess{};
    textureBufferAccess.name = "ReadAssimp_inputTexture";
    textureBufferAccess.type = ResourceType::Texture;
    textureBufferAccess.access = AccessType::Read;
    textureBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(textureBufferAccess);

    ResourceAccess gBufferAccess{};
    gBufferAccess.name = "gBuffer";
    gBufferAccess.type = ResourceType::FrameBuffer;
    gBufferAccess.access = AccessType::Write;
    gBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(gBufferAccess);

    /* Ports */
    Port cameraIn(this);

    Port geoOut(this); 

    _inPorts.emplace("camera", cameraIn);

    _outPorts.emplace("geo", geoOut);
};

void ReadAssimp::BuildKnobs()
{
    if (auto knob = _knobManager->CreateKnob(KnobType::String, kModelFilePathKnobName)) {
        KnobUI ui = {.displayName = "Model FilePath:",
                     .isHidden = false,
                     .tooltip = "File Path to the Asset to Read." };

        KnobInfo info {.type = KnobType::String,
                       .name = kModelFilePathKnobName,
                       .ui = ui,
                       .canAnimate = false };
        _pFilePathKnob = std::get_if<StringKnob>(knob);
        _pFilePathKnob->SetInfo(info);
    };

    if (auto knob = _knobManager->CreateKnob(KnobType::String, kTextureFilePathKnobName)) {
        KnobUI ui = {.displayName = "Texture FilePath:",
                     .isHidden = false,
                     .tooltip = "File Path to the Asset to Read." };

        KnobInfo info {.type = KnobType::String,
                       .name = kTextureFilePathKnobName,
                       .ui = ui,
                       .canAnimate = false };
        _pTextureFilePathKnob = std::get_if<StringKnob>(knob);
        _pTextureFilePathKnob->SetInfo(info);
    };
};

void ReadAssimp::OnKnobChange(KnobGeneric* knob)
 {
    auto type = static_cast<KnobType>(knob->index());
    if (type == KnobType::String) {
        auto knobPtr = std::get_if<StringKnob>(knob); 
        if (knobPtr == _pFilePathKnob ||
            knobPtr == _pTextureFilePathKnob) {
            ReadFile(_pFilePathKnob->GetValue(), _pTextureFilePathKnob->GetValue());
        };
    }
 };

void ReadAssimp::ReadFile(const std::string& modelFilePath, const std::string& textureFilePath)
{
    _modelFilePath = modelFilePath;
    _textureFilePath = textureFilePath;
    const aiScene* scene = _importer.ReadFile(_modelFilePath,
                                            aiProcess_CalcTangentSpace      |
                                            aiProcess_Triangulate           |
                                            aiProcess_SortByPType           |
                                            aiProcess_FlipUVs);
    
    if (scene == nullptr)
    {
        auto msg = "ReadAssimp: Failed to Read Model: " + std::string(_importer.GetErrorString());
        Logger::LogError(msg);
    }

    /* We want to look at every node. If the node has meshes than we will store it in a vector.
    Once we Checked the whole array. Then Load and store of all the vertex data. */
    std::vector<aiNode*> meshNodes = findNodesContainingMeshes(scene->mRootNode);

    struct VertexDetails
    {
        bool hasPositions = {false};
        bool hasNormals = {false};
        bool hasTextureUVs = {false};
    };

    VertexDetails details{};
    for (auto node : meshNodes)
    {
        auto meshCount = node->mNumMeshes;
        for (int i = 0; i < meshCount; i++)
        {
            auto index = node->mMeshes[i];
            auto mesh = scene->mMeshes[index];

            if (mesh->HasPositions() && !details.hasPositions)
            {
                details.hasPositions = true;

                BufferElement positionsElement{};
                positionsElement.count = 3;
                positionsElement.type = BufferElementType::Float;
                positionsElement.normalized = false;
                _vertexLayout.AddBufferElement(positionsElement);
            };

            if (mesh->HasNormals() && !details.hasNormals)
            {
                details.hasNormals = true;

                BufferElement normalsElement{};
                normalsElement.count = 3;
                normalsElement.type = BufferElementType::Float;
                normalsElement.normalized = false;
                _vertexLayout.AddBufferElement(normalsElement);
            };
            
            auto UV_Count = mesh->GetNumUVChannels();
            for (int i = 0; i < UV_Count; i++)
            {
                if (mesh->HasTextureCoords(i) && !details.hasTextureUVs)
                {
                    details.hasTextureUVs = true;

                    BufferElement textureUVElement{};
                    textureUVElement.count = 3;
                    textureUVElement.type = BufferElementType::Float;
                    textureUVElement.normalized = false;
                    _vertexLayout.AddBufferElement(textureUVElement);
                };
            };

            auto vertexCount = mesh->mNumVertices;
            for (int i = 0; i < vertexCount; i++)
            {
                if (details.hasPositions)
                {
                    auto position = mesh->mVertices[i];
                    _vertexArray.push_back(glm::vec3(position.x, position.y, position.z));
                };
                
                if (details.hasNormals)
                {
                    auto normal = mesh->mNormals[i];
                    _vertexArray.push_back(glm::vec3(normal.x, normal.y, normal.z));
                };

                if (details.hasTextureUVs)
                {
                    for (int j = 0; j < UV_Count; j++)
                    {
                        if (mesh->HasTextureCoords(j))
                        {
                            auto uv = mesh->mTextureCoords[j][i];
                            _vertexArray.push_back(glm::vec3(uv.x, uv.y, uv.z));
                        };
                    };
                };

                _indexArray.push_back((_globalBufferIndices + i));
            };

            _globalBufferIndices = _globalBufferIndices + vertexCount;

            /* Get Model Transforms */
            aiMatrix4x4 aiTransforms = node->mTransformation;
            glm::mat4 glmTransform;
            glmTransform[0][0] = aiTransforms.a1;
            glmTransform[0][1] = aiTransforms.a2;
            glmTransform[0][2] = aiTransforms.a3;
            glmTransform[0][3] = aiTransforms.a4;

            glmTransform[1][0] = aiTransforms.b1;
            glmTransform[1][1] = aiTransforms.b2;
            glmTransform[1][2] = aiTransforms.b3;
            glmTransform[1][3] = aiTransforms.b4;

            glmTransform[2][0] = aiTransforms.c1;
            glmTransform[2][1] = aiTransforms.c2;
            glmTransform[2][2] = aiTransforms.c3;
            glmTransform[2][3] = aiTransforms.c4;

            glmTransform[3][0] = aiTransforms.d1;
            glmTransform[3][1] = aiTransforms.d2;
            glmTransform[3][2] = aiTransforms.d3;
            glmTransform[3][3] = aiTransforms.d4;

            _transformArray.data[i] = glmTransform;
            _meshCount++;
        };
    };

    _transformArray.meshCount = _meshCount;

    BufferElement indexElement{};
    indexElement.count = 1;
    indexElement.type = BufferElementType::Int;
    indexElement.normalized = false;
    _indexLayout.AddBufferElement(indexElement);

    BufferElement matrixElement;
    matrixElement.count = (4 * 4) * MAX_MESH_COUNT;
    matrixElement.type = BufferElementType::Float;
    matrixElement.normalized = false;
    _transformLayout.AddBufferElement(matrixElement); /* Model */
    _transformLayout.AddBufferElement(indexElement); /* Mesh Count */
};

std::vector<aiNode*> ReadAssimp::findNodesContainingMeshes(aiNode* node)
{
    std::vector<aiNode*> output;
    auto dfs = [&](auto&& self, aiNode* node)
    {
        if (node == nullptr)
        {
            return nullptr;
        };

        if (node->mNumChildren > 0)
        {
            for (int i = 0; i < node->mNumChildren; i++)
            {
                self(self, node->mChildren[i]);
            };
        };

        if (node->mNumMeshes > 0)
        {
            output.push_back(node);
        };

        return nullptr;
    };

    auto result = dfs(dfs, node);

    return output;
};

void ReadAssimp::AllocateResources(RenderEngine*  renderEngine)
{
    ShaderCreateInfo shaderInfo{};
    shaderInfo.name = "ReadAssimp";
    shaderInfo.pContext = renderEngine->GetContextPtr();
    shaderInfo.pDevice = renderEngine->GetDevicePtr();
    shaderInfo.type = ShaderType::Graphics;
    shaderInfo.sourceFilepath = "./Shaders/ReadAssimp.glsl";
    renderEngine->GetShaderLibraryPtr()->CreateShader(shaderInfo);
    _pShader = renderEngine->GetShaderLibraryPtr()->GetShader("ReadAssimp");

    BufferCreateInfo vertexInfo{};
    vertexInfo.context = renderEngine->GetContextPtr();
    vertexInfo.device = renderEngine->GetDevicePtr();
    vertexInfo.count = _globalBufferIndices;
    vertexInfo.type = BufferType::Vertex;
    vertexInfo.layout = _vertexLayout;
    vertexInfo.data = _vertexArray.data();
    renderEngine->StoreBufferPtr("ReadAssimp_vertexBuffer", Buffer::CreateBuffer(vertexInfo));
    _vertexArray.clear();

    BufferCreateInfo indexInfo{};
    indexInfo.context = renderEngine->GetContextPtr();
    indexInfo.device = renderEngine->GetDevicePtr();
    indexInfo.count = _globalBufferIndices;
    indexInfo.type = BufferType::Index;
    indexInfo.layout = _indexLayout;
    indexInfo.data = _indexArray.data();
    renderEngine->StoreBufferPtr("ReadAssimp_indexBuffer", Buffer::CreateBuffer(indexInfo));
    _indexArray.clear();

    BufferCreateInfo transformsUniformInfo{};
    transformsUniformInfo.context = renderEngine->GetContextPtr();
    transformsUniformInfo.device = renderEngine->GetDevicePtr();
    transformsUniformInfo.count = 1;
    transformsUniformInfo.type = BufferType::Uniform;
    transformsUniformInfo.layout = _transformLayout;
    transformsUniformInfo.data = nullptr;
    renderEngine->StoreBufferPtr("ReadAssimp_transformsBuffer", Buffer::CreateBuffer(transformsUniformInfo));
    renderEngine->GetBufferPtr("ReadAssimp_transformsBuffer")->UploadToDevice(&_transformArray);

    TextureCreateInfo inputTextureInfo{};
    inputTextureInfo.pContext = renderEngine->GetContextPtr();
    inputTextureInfo.pDevice = renderEngine->GetDevicePtr();
    inputTextureInfo.filePath = _textureFilePath;
    renderEngine->StoreTexturePtr("ReadAssimp_inputTexture", Texture::CreateTexture(inputTextureInfo));

    FrameBufferCreateInfo gBufferInfo{};
    gBufferInfo.name = "gBuffer";
    gBufferInfo.pContext = renderEngine->GetContextPtr();
    gBufferInfo.pDevice = renderEngine->GetDevicePtr();
    gBufferInfo.isSwapChain = false;
    gBufferInfo.height = renderEngine->GetTargetFrameBufferPtr()->GetHeight();
    gBufferInfo.width = renderEngine->GetTargetFrameBufferPtr()->GetWidth();
    renderEngine->StoreFrameBufferPtr("gBuffer", FrameBuffer::CreateFrameBuffer(gBufferInfo));
    renderEngine->GetFrameBufferPtr("gBuffer")->CreateLayer("gColor", FrameBufferPixelFormat::COLOR_RGBA_8888);
    renderEngine->GetFrameBufferPtr("gBuffer")->CreateLayer("gPosition", FrameBufferPixelFormat::COLOR_RGBA_8888);
    renderEngine->GetFrameBufferPtr("gBuffer")->CreateLayer("gNormal", FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat);
    renderEngine->GetFrameBufferPtr("gBuffer")->CreateLayer("depthStencil", FrameBufferPixelFormat::DEPTH_STENCIL_D32_S8);

    /* Passing the gBuffer into the Port */
    _outPorts.at("geo").SetOutgoingResource(ResourceType::FrameBuffer, "gBuffer");
};

void ReadAssimp::BindResources(RenderEngine* renderEngine)
{
    auto connectedPorts = _inPorts.at("camera").GetConnectedPorts();
    for (auto port : connectedPorts)
    {
        if (port->GetIncomingResourceType() == ResourceType::Uniform)
        {
            auto cameraBufferName = port->GetIncomingResourceName();
            renderEngine->GetBufferPtr(cameraBufferName)->Bind(_pRenderContext);
        };
    };

    renderEngine->GetFrameBufferPtr("gBuffer")->Bind(_pRenderContext, true);
    renderEngine->GetBufferPtr("ReadAssimp_vertexBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("ReadAssimp_indexBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("ReadAssimp_transformsBuffer")->Bind(_pRenderContext);
    renderEngine->GetTexturePtr("ReadAssimp_inputTexture")->Bind(_pRenderContext);
    renderEngine->GetShaderLibraryPtr()->GetShader("ReadAssimp")->Bind(_pRenderContext);
};

void ReadAssimp::Execute(RenderEngine* renderEngine)
{
    renderEngine->GetCommandPtr()->Draw(_pRenderContext);
};

};