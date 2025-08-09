#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

void RenderPass::AppendRenderPass(RenderPass* pRenderPass)
{
    _nextsPasses.push_back(pRenderPass);
};

std::vector<ResourceAccess> RenderPass::GetResourcesAccess()
{
    return _resourcesAccess;
};

std::vector<RenderPass*> RenderPass::GetNextsRenderPasses()
{
    return _nextsPasses;
};

};