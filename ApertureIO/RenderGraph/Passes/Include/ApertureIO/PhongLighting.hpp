#pragma once

#include "ApertureIO/RenderPass.hpp"

#include <glm/glm.hpp>
#include <vector>

const uint32_t MAX_LIGHTS = 128;

namespace Aio
{

struct PointLight
{
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float _pad1; float _pad2; float _pad3;
};

struct LightsMetadata
{
    uint32_t numOfLights;
    float specular;
    float diffuse;
    float ambient;
    float shininess;
};

class PhongLighting : public RenderPass
{
public:
    PhongLighting();
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */

    void GeneratePointLights(uint32_t numOfLights, float bbox_X, float bbox_Y, float bbox_Z);
    void UpdateLightsBuffer(RenderEngine* renderEngine);

private:
    std::vector<PointLight> _lights;
    uint32_t _numOfLights;
};

};