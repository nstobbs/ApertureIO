#ifndef H_VULKANSHADER
#define H_VULKANSHADER

#include "VulkanDevice.hpp"
#include "VulkanCommon.hpp"
#include "../Base/Shader.hpp"

#include <vector>

namespace Aio {

class VulkanShader : public Shader
{
    public:
    VulkanShader(ShaderCreateInfo& createInfo);

    ~VulkanShader();

    void Bind() override;
    void Unbind() override;

    void rebuildShader() override;

    void SetVec4(std::string name, glm::vec4 value) override;

    private:
    std::vector<VkShaderModule> _shaderModules;

    VkViewport _viewport;
    VkRect2D _scissor;

    VkPipeline _pipeline;
    VkPipelineLayout _layout;

    VulkanDevice* _pDevice;

    VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

    int hash; //TODO: might be useful to use hashing so that we know when to rebuild the graphics pipeline.
};

}; // End of Aio namespace

#endif