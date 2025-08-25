#include "ApertureIO/RenderPassFactory.hpp"

/* Include All RenderPasses Here  */
#include "ApertureIO/BasicRenderPass.hpp"
#include "ApertureIO/ReadAssimp.hpp"

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
    };
};

};