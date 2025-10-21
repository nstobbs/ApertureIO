#pragma once

#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

class ViewportGrid : public RenderPass
{
public:
    ViewportGrid();
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */
};

};

