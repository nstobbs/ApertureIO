#pragma once

#include <cstdint> //?? first time ive ever included this for uint32_t 

/* TODO: Consider replacing the uint32_t for a uint16_t instead.
This would means we would have less buffers and textures on each device.
But would allow us to double the number of handles that 
we can give to each shader via the push constants. */

namespace Aio
{
    using BufferHandle = uint32_t; // Storage Buffers and Uniform Buffers Use this Handle
    using TextureHandle = uint32_t;
};