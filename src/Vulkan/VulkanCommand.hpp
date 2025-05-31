#ifndef H_VULKANCOMMAND
#define H_VULKANCOMMAND

#include <vector>

#include "../Base/Command.hpp"
#include "../Base/Context.hpp"
#include "../Base/RenderContext.hpp"

#include "VulkanCommon.hpp"
#include "VulkanBuffers.hpp"

namespace Aio {

class VulkanCommand : public Command
{
    public:
    VulkanCommand(Context* context);
    
    void StartCommand(RenderContext& renderContext) override;
    void EndCommand(RenderContext& renderContext) override;
    void Draw(RenderContext& renderContext) override;
    void DrawInstance(RenderContext& renderContext) override;
    void DispatchCompute(RenderContext& renderContext) override;
    void Clear(RenderContext& renderContext) override; //TODO maybe move this into framebuffer??

    static void CopyBuffer(VulkanDevice* pDevice, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t size);

    private:
    void createCommandBuffers(VulkanDevice* pDevice);

    static VkCommandBuffer beginSingleTimeCommandBuffer(VulkanDevice* pDevice);
    static void endSingleTimeCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer commandBuffer);
    
    Context* _pContext;
};
} // End of Aio Namespace

#endif