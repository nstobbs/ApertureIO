#pragma once

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Command.hpp"
#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Texture.hpp"

#include "ApertureIO/Logger.hpp"

#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <vector>

namespace Aio {
    class Buffer;
    class Shader;
    class FrameBuffer;
}

/* singleton object that store the currently Bound objects
to be used for rendering the current object. */
namespace Aio {

class RenderContext
{
    public:
    size_t GetHash();
    
    void IsPaused(); // TODO: not a good name since it doesn't return an bool anymore.
    void PauseRendering();
    void UnpauseRendering();

    Device* _Device;
    Context* _Context;
    
    Buffer* _VertexBuffer;
    Buffer* _IndexBuffer;

    std::vector<Buffer*> _UniformBuffers;
    std::vector<Buffer*> _StorageBuffers;
    std::vector<Texture*> _Textures;

    Shader* _Shader;
    FrameBuffer* _TargetFrameBuffer;

    private:

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> _isReloading = {false};

    bool _valid = {false};
    size_t _hash;
};

};