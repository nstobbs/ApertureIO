#include "ApertureIO/RenderPassFactory.hpp"

/* Include All RenderPasses Here  */
#include "ApertureIO/BasicRenderPass.hpp"
#include "ApertureIO/ReadAssimp.hpp"
#include "ApertureIO/PhongLighting.hpp"
#include "ApertureIO/AsciiImage.hpp"

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
    };
};

};