#include "ApertureIO/RenderPassFactory.hpp"

/* Include All RenderPasses Here  */
#include "ApertureIO/BasicRenderPass.hpp"

namespace Aio
{

UniquePtr<RenderPass> RenderPassFactory::CreateRenderPass(const std::string& name)
{

    if (name == "BasicRenderPass")
    {
        return std::make_unique<BasicRenderPass>();
    };
};

};