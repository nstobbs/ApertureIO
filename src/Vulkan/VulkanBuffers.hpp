#ifndef H_VULKANBUFFERS
#define H_VULKANBUFFERS

#include "VulkanCommon.hpp"
#include "VulkanDevice.hpp"

#include "../Base/Device.hpp"
#include "../Base/Buffers.hpp"
#include "../Base/BufferLayout.hpp"
#include "../Base/RenderContext.hpp"

namespace Aio {

class VulkanBuffer : public Buffer
{
    public:
    ~VulkanBuffer();
    VulkanBuffer(BufferCreateInfo* createInfo);

    void UploadToDevice(void* data);

    /* Might need to function to rebuild the buffer */
    void rebuildBuffer(); //destory and rebuild the buffer.

    void Bind(RenderContext& renderContext) override;
    void Unbind() override;

    private:
    VulkanDevice* _pDevice;

    BufferType _type;
    BufferLayout _layout;
    
    VkBuffer _buffer;
    VmaAllocation _allocation;

    uint32_t _size;
    void* _pData;
};
}; // End of Aio Namespace
#endif