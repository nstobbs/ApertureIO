#pragma once
#include "ApertureIO/RenderPass.hpp"

#include <string>

namespace Aio
{

class MergeLayers : public RenderPass
{
public:
    MergeLayers();
    void BuildKnobs() override;
    void OnKnobChange(KnobGeneric* knob) override;
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */

    void TargetLayer(const std::string& name);

private:
    std::string _targetLayerA = {"None"};
    std::string _targetLayerB = {"None"};

    /* Knobs */
    StringKnob* _targetLayerKnob;
};

};