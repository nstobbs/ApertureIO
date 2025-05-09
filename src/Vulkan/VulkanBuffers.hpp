#ifndef H_VULKANBUFFERS
#define H_VULKANBUFFERS

#include "VulkanCommon.hpp"
#include "VulkanDevice.hpp"

#include "../Base/Device.hpp"
#include "../Base/Buffers.hpp"
#include "../Base/BufferLayout.hpp"

namespace ApertureIO {

class VulkanBuffer : public Buffer
{
    public:
    VulkanBuffer(Device* device, void* pData, BufferLayout layout, uint32_t vertexCount, bool hostAccess);

    void Bind() override;
    void Unbind() override;

    private:
    VulkanDevice* _pDevice;

    BufferLayout _layout;
    VkBuffer _buffer;
    VmaAllocation _allocation;
};
}; // End of ApertureIO Namespace
#endif