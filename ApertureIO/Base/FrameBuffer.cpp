#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

namespace Aio {

UniquePtr<FrameBuffer> FrameBuffer::CreateFrameBuffer(const FrameBufferCreateInfo& createInfo)
{
    switch(createInfo.pContext->getRendererAPI())
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanFrameBuffer>(createInfo);
    }
};

FrameBufferPixelFormat FrameBuffer::GetLayerPixelFormat(const std::string& name)
{
    return _layersMap.at(name);
};

void FrameBuffer::CreateLayer(const std::string& name, FrameBufferPixelFormat format)
{
    _layerCount++;
    _layersMap.emplace(name, format);
    _layerOrder.push_back(name);
};

std::vector<std::string> FrameBuffer::GetLayerNames()
{
    return _layerOrder;
};

uint32_t FrameBuffer::GetWidth()
{
    return _width;
};

uint32_t FrameBuffer::GetHeight()
{
    return _height;
};

uint32_t FrameBuffer::GetLayerCount()
{
    return _layerCount;
};

} // End Aio namespace

