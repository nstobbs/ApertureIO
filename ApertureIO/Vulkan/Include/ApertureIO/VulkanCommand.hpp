#pragma once

#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanBuffers.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"
#include "ApertureIO/VulkanShader.hpp"

#include "ApertureIO/Command.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"

#include <vector>
#include <array>

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