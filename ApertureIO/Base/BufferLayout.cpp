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
            case Float:
                stride += sizeof(float) * element.count;
                break;
            case Int:
                stride += sizeof(uint32_t) * element.count;
                break;
            case Bool:
                stride += sizeof(bool) * element.count;
                break;
        }
    };

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