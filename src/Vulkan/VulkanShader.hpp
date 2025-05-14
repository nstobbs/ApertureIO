#ifndef H_VULKANSHADER
#define H_VULKANSHADER

#include "VulkanCommon.hpp"
#include "../Base/Shader.hpp"

namespace Aio {

class VulkanShader : public Shader
{
    public:
    VulkanShader(ShaderComputeCreateInfo& createInfo);
    VulkanShader(ShaderGraphicsCreateInfo& createInfo);

    void Bind() override;
    void Unbind() override;

    void SetVec4(std::string name, glm::vec4 value) override;

    private:

    VkPipeline _pipeline;
    VkPipelineLayout _layout;

    int hash; //TODO: might be useful to use hashing so that we know when to rebuild the graphics pipeline.
};

}; // End of Aio namespace

#endif