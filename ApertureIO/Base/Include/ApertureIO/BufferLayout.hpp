#pragma once

#include <glm/glm.hpp>
#include <vector> 

namespace Aio {


//TODO: Used to get Default Layout.
enum class BufferBytesLayout
{
    UNF32, // Float
    UNF32F32F32, // Vec3 Float
    UNF32F32F32F32, // Vec4 Float
};


enum class BufferElementType
{
    Float = 0,
    Int = 1,
    Bool = 2
};

struct BufferElement
{
    BufferElementType type;
    uint32_t count;
    bool normalized;
};

class BufferLayout
{
public:
    void AddBufferElement(BufferElement element);
    uint32_t GetElementCount();
    size_t GetStride();
    BufferElement GetBufferElementAt(uint32_t index);

private:
    std::vector<BufferElement> _elements;
    uint32_t _stride;

    size_t calculateStride();
    size_t calculateStride_std140();
};

};