#include "ApertureIO/RenderPassFactory.hpp"

/* Include All RenderPasses Here  */
#include "ApertureIO/BasicRenderPass.hpp"

/* Reader Passes */
#include "ApertureIO/ReadAssimp.hpp"

/* Post Processing Passes */
#include "ApertureIO/PhongLighting.hpp"
#include "ApertureIO/AsciiImage.hpp"

/* Utility Passes */
#include "ApertureIO/CameraManager.hpp"
#include "ApertureIO/MergeLayers.hpp"

/* HUD Passes */
#include "ApertureIO/ViewportGrid.hpp"

namespace Aio
{

UniquePtr<RenderPass> RenderPassFactory::CreateRenderPass(const std::string& name)
{
    UniquePtr<RenderPass> renderPass;

    if (name == "BasicRenderPass")
    {
        renderPass = std::make_unique<BasicRenderPass>();
    } 
    else if (name == "ReadAssimp")
    {
        renderPass = std::make_unique<ReadAssimp>();
    }
    else if (name == "PhongLighting")
    {
        renderPass = std::make_unique<PhongLighting>();
    }
    else if (name == "AsciiImage")
    {
        renderPass = std::make_unique<AsciiImage>();
    }
    else if (name == "CameraManager")
    {
        renderPass =  std::make_unique<CameraManager>();
    }
    else if (name == "MergeLayers")
    {
        renderPass =  std::make_unique<MergeLayers>();
    }
    else if (name == "ViewportGrid")
    {
        renderPass =  std::make_unique<ViewportGrid>();
    };

    renderPass->BuildKnobs();
    return renderPass;
};
};
