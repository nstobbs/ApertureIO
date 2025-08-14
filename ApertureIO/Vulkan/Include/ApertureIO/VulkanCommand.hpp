#pragma once

#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanBuffers.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"
#include "ApertureIO/VulkanShader.hpp"

#include "ApertureIO/Command.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Handles.hpp"

#include <vector>
#include <array>

namespace Aio {

class VulkanCommand : public Command
{
public:
    VulkanCommand(WeakPtr<Context> context, WeakPtr<Device> device);
    
    void BeginFrame(RenderContext& renderContext) override;
    void EndFrame(RenderContext& renderContext) override;
    void Draw(RenderContext& renderContext) override;

    //void DrawInstance(RenderContext& renderContext) override;
    //void DispatchCompute(RenderContext& renderContext) override;
    //void Clear(RenderContext& renderContext) override; //TODO maybe move this into framebuffer??

    /* Static Functions */
    static void CopyBuffer(WeakPtr<VulkanDevice> pDevice, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t size);
    static void CopyBufferToImage(WeakPtr<VulkanDevice> pDevice, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height);
    static VkCommandBuffer beginSingleTimeCommandBuffer(WeakPtr<VulkanDevice> pDevice);
    static void endSingleTimeCommandBuffer(WeakPtr<VulkanDevice> pDevice, VkCommandBuffer commandBuffer);

private:
    
    uint32_t _imageIndex;
    WeakPtr<Context> _pContext;
    WeakPtr<VulkanDevice> _pDevice;
};
} // End of Aio Namespace