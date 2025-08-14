#pragma once

#include "ApertureIO/Logger.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Command.hpp"
#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Texture.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace Aio {
}

/* singleton object that store the currently Bound objects
to be used for rendering the current object. */
namespace Aio {

class Buffer;
class Shader;
class FrameBuffer;
class Texture;

class RenderContext
{
public:
    size_t GetHash();
    
    void IsPaused(); // TODO: not a good name since it doesn't return an bool anymore.
    void PauseRendering();
    void UnpauseRendering();

    WeakPtr<Device> _Device;
    WeakPtr<Context> _Context;
    
    WeakPtr<Buffer> _VertexBuffer;
    WeakPtr<Buffer> _IndexBuffer;

    std::vector<WeakPtr<Buffer>> _UniformBuffers;
    std::vector<WeakPtr<Buffer>> _StorageBuffers;
    std::vector<WeakPtr<Buffer>> _Textures;

    WeakPtr<Shader> _Shader;
    WeakPtr<FrameBuffer> _TargetFrameBuffer;
private:

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> _isReloading = {false};

    bool _valid = {false};
    size_t _hash;
};

};