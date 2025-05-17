#include "Shader.hpp"

#include "../Vulkan/VulkanShader.hpp"

namespace Aio {
    
Shader* CreateShader(ShaderCreateInfo& createInfo)
{
    auto API =  createInfo.pContext->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            return new VulkanShader(createInfo);
    }
};

}