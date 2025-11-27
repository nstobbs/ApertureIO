#pragma once

#include "ApertureIO/RenderPass.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <vector>

const uint32_t MAX_MESH_COUNT = 32;

namespace Aio
{

struct Transforms
{
    glm::mat4 data[MAX_MESH_COUNT] = {glm::mat4(1.0f)};
    uint32_t meshCount = {0};
};

class ReadAssimp : public RenderPass
{
public:
    ReadAssimp();
    void BuildKnobs() override;
    void OnKnobChange(KnobGeneric* knob) override;
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */
    
    void ReadFile(const std::string& modelFilePath, const std::string& textureFilePath);
    void rotateModel(RenderEngine* renderEngine);

private:
    /* Knobs */
    StringKnob* _filePathKnob = {nullptr};
    StringKnob* _textureFilePathKnob = {nullptr};

    std::vector<aiNode*> findNodesContainingMeshes(aiNode* node);

    std::string _modelFilePath;
    std::string _textureFilePath;
    Assimp::Importer _importer;
    uint32_t _meshCount = {0};

    uint32_t _globalBufferIndices = {0};

    BufferLayout _vertexLayout;
    BufferLayout _indexLayout;
    BufferLayout _transformLayout;

    std::vector<glm::vec3> _vertexArray;
    std::vector<uint32_t> _indexArray;
    Transforms _transformArray;
};

};

