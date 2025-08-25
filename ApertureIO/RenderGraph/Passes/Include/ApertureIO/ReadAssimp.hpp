#pragma once

#include "ApertureIO/RenderPass.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <vector>

namespace Aio
{

struct Camera
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

class ReadAssimp : public RenderPass
{
public:
    ReadAssimp();
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */
    
    void ReadFile(const std::string& modelFilePath, const std::string& textureFilePath);
    void UpdateCamera(RenderEngine* RenderEngine, Camera cam);

private:
    std::vector<aiNode*> findNodesContainingMeshes(aiNode* node);

    std::string _modelFilePath;
    std::string _textureFilePath;
    Assimp::Importer _importer;

    uint32_t _globalBufferIndices = {0};

    BufferLayout _vertexLayout;
    BufferLayout _indexLayout;
    BufferLayout _cameraUniformLayout;

    std::vector<glm::vec3> _vertexArray;
    std::vector<uint32_t> _indexArray;
    Camera _camera;
};

};

