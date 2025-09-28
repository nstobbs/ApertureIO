#include <ApertureIO/ReadAssimp.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

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

    ResourceAccess uniformBufferAccess{};
    uniformBufferAccess.name = "ReadAssimp_cameraBuffer";
    uniformBufferAccess.type = ResourceType::Uniform;
    uniformBufferAccess.access = AccessType::Both;
    uniformBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(uniformBufferAccess);
    
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
    Port imageOut(this);
    Port geoOut(this);

    _outPorts.emplace("image", imageOut);
    _outPorts.emplace("geo", geoOut);
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
    Once we Checked the whole array. Then Load ans store of all the vertex data. */
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
        };
    };

    BufferElement indexElement{};
    indexElement.count = 1;
    indexElement.type = BufferElementType::Int;
    indexElement.normalized = false;
    _indexLayout.AddBufferElement(indexElement);

    BufferElement matrixElement;
    matrixElement.count = 4 * 4;
    matrixElement.type = BufferElementType::Float;
    matrixElement.normalized = false;
    _cameraUniformLayout.AddBufferElement(matrixElement); /* Model */
    _cameraUniformLayout.AddBufferElement(matrixElement); /* View */
    _cameraUniformLayout.AddBufferElement(matrixElement); /* Projection */

    _camera.model = glm::mat4(1.0f);
    _camera.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

    _camera.projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    _camera.projection[1][1] *= -1.0f;
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

void ReadAssimp::rotateModel()
{
    _camera.model = glm::rotate(_camera.model, glm::radians(0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
};

void ReadAssimp::UpdateCamera(RenderEngine* renderEngine, Camera cam)
{
    _camera = cam;
    renderEngine->GetBufferPtr("ReadAssimp_cameraBuffer")->UploadToDevice(&_camera);
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

    BufferCreateInfo cameraUniformInfo{};
    cameraUniformInfo.context = renderEngine->GetContextPtr();
    cameraUniformInfo.device = renderEngine->GetDevicePtr();
    cameraUniformInfo.count = 1;
    cameraUniformInfo.type = BufferType::Uniform;
    cameraUniformInfo.layout = _cameraUniformLayout;
    cameraUniformInfo.data = nullptr;
    renderEngine->StoreBufferPtr("ReadAssimp_cameraBuffer", Buffer::CreateBuffer(cameraUniformInfo));
    renderEngine->GetBufferPtr("ReadAssimp_cameraBuffer")->UploadToDevice(&_camera);

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
    renderEngine->GetFrameBufferPtr("gBuffer")->CreateLayer("gNormal", FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat);
};

void ReadAssimp::BindResources(RenderEngine* renderEngine)
{
    renderEngine->GetFrameBufferPtr("gBuffer")->Bind(_pRenderContext, true);
    renderEngine->GetBufferPtr("ReadAssimp_vertexBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("ReadAssimp_indexBuffer")->Bind(_pRenderContext);
    renderEngine->GetBufferPtr("ReadAssimp_cameraBuffer")->Bind(_pRenderContext);
    renderEngine->GetTexturePtr("ReadAssimp_inputTexture")->Bind(_pRenderContext);
    renderEngine->GetShaderLibraryPtr()->GetShader("ReadAssimp")->Bind(_pRenderContext);
};

void ReadAssimp::Execute(RenderEngine* renderEngine)
{
    renderEngine->GetCommandPtr()->Draw(_pRenderContext);
    rotateModel();
    UpdateCamera(renderEngine, _camera); //TODO: this is dumb function, check args
};

};