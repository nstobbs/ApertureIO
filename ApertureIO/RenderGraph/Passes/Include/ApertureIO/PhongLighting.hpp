#pragma once

#include "ApertureIO/RenderPass.hpp"

#include <glm/glm.hpp>
#include <vector>

const uint32_t MAX_LIGHTS = 1024;

namespace Aio
{

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

struct LightsMetadata
{
    uint32_t width;
    uint32_t height;
    uint32_t numOfLights;
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