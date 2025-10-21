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

    if (name == "BasicRenderPass")
    {
        return std::make_unique<BasicRenderPass>();
    } 
    else if (name == "ReadAssimp")
    {
        return std::make_unique<ReadAssimp>();
    }
    else if (name == "PhongLighting")
    {
        return std::make_unique<PhongLighting>();
    }
    else if (name == "AsciiImage")
    {
        return std::make_unique<AsciiImage>();
    }
    else if (name == "CameraManager")
    {
        return std::make_unique<CameraManager>();
    }
    else if (name == "MergeLayers")
    {
        return std::make_unique<MergeLayers>();
    }
    else if (name == "ViewportGrid")
    {
        return std::make_unique<ViewportGrid>();
    };
};
};
