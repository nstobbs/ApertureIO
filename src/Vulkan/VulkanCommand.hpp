#ifndef H_VULKANCOMMAND
#define H_VULKANCOMMAND

#include <vector>

#include "VulkanCommon.hpp"
#include "../Base/Command.hpp"

namespace ApertureIO {

class VulkanCommand : public Command
{
    public:
    void StartCommand(Device* pDevice) override;
    void EndCommand(Device* pDevice) override;
    void Draw(Device* pDevice) override;
    void DrawInstance(Device* pDevice) override;
    void DispatchCompute(Device* pDevice) override;
    void Clear(Device* pDevice) override; //TODO maybe move this into framebuffer??

    private:
    VkCommandPool _commandPool;
    std::vector<VkCommandBuffer> _commandBuffers;

    std::vector<VkFence> _fences;
    std::vector<VkSemaphore> _semaphores;
    
};
} // End of ApertureIO Namespace

#endif