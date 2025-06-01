#ifndef H_VULKANCOMMAND
#define H_VULKANCOMMAND

#include <vector>
#include <array>

#include "../Base/Command.hpp"
#include "../Base/Context.hpp"
#include "../Base/RenderContext.hpp"

#include "VulkanCommon.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBuffers.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanShader.hpp"


namespace Aio {

class VulkanCommand : public Command
{
    public:
    VulkanCommand(Context* context, Device* device);
    
    void StartCommand(RenderContext& renderContext) override;
    //void EndCommand(RenderContext& renderContext) override;
    void Draw(RenderContext& renderContext) override;
    //void DrawInstance(RenderContext& renderContext) override;
    //void DispatchCompute(RenderContext& renderContext) override;
    //void Clear(RenderContext& renderContext) override; //TODO maybe move this into framebuffer??

    static void CopyBuffer(VulkanDevice* pDevice, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t size);

    private:
    void createCommandBuffers(VulkanDevice* pDevice);

    static VkCommandBuffer beginSingleTimeCommandBuffer(VulkanDevice* pDevice);
    static void endSingleTimeCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer commandBuffer);
    
    Context* _pContext;
    VulkanDevice* _pDevice;
};
} // End of Aio Namespace

#endif