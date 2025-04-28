#ifndef H_VULAKNFRAMEBUFFER
#define H_VULAKNFRAMEBUFFER

#include "../Base/FrameBuffer.hpp"
#include "../Base/Context.hpp"
#include "../Base/Device.hpp"

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"

namespace ApertureIO {

class VulkanFrameBuffer : public FrameBuffer
{
    public:
    VulkanFrameBuffer(Device* device, Context* context);
    VulkanFrameBuffer(Device* device);

    bool init() override;

    private:
    VulkanDevice* _pDevice;
    VulkanContext* _pContext;
}; 
}

#endif