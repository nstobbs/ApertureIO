#include "ApertureIO/BufferLayout.hpp"

namespace Aio {

void BufferLayout::AddBufferElement(BufferElement element)
{
    _elements.push_back(element);
};

/* Getter Functions */
uint32_t BufferLayout::GetElementCount()
{
    return static_cast<uint32_t>(_elements.size());
};

size_t BufferLayout::calculateStride()
{
    size_t stride = 0;
    for (auto element : _elements)
    {
        switch(element.type)
        {
            case BufferElementType::Float:
                stride += sizeof(float) * element.count;
                break;
            case BufferElementType::Int:
                stride += sizeof(uint32_t) * element.count;
                break;
            case BufferElementType::Bool:
                stride += sizeof(bool) * element.count;
                break;
        }
    };
    
    return stride;
};

size_t BufferLayout::calculateStride_std140()
{
    size_t stride = 0;
    for (auto element : _elements)
    {
        switch (element.type)
        {
            case BufferElementType::Float:
                if (element.count == 3)
                {
                    stride += sizeof(float) * 4;
                }
                else
                {
                    stride += sizeof(float) * element.count;
                }
                break;
            case BufferElementType::Int:
                stride += sizeof(uint32_t) * element.count;
                break;
            case BufferElementType::Bool:
                stride += sizeof(bool) * element.count;
                break;
        }
    }

    return stride;
};

size_t BufferLayout::GetStride()
{
    return calculateStride();
};

BufferElement BufferLayout::GetBufferElementAt(uint32_t index)
{
    return _elements.at(index);
};
} // End of Aio namespace