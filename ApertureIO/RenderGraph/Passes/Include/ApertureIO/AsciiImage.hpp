#pragma once

#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

struct AsciiImageSettings
{
    uint32_t _mapSize = {8};
    uint32_t charCount = {9};
    float _brightness = {1.0f};
    bool _matrixMode = false;
};

class AsciiImage : public RenderPass
{
public:
    AsciiImage();
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */

    void updateUniformBuffer(RenderEngine* renderEngine);

private:
    AsciiImageSettings _settings;
};

};