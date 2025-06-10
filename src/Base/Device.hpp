#ifndef H_DEVICE
#define H_DEVICE

#include "Context.hpp"
#include "Command.hpp"
#include "Handles.hpp"

#include <vector>

//TODO: dont know why this fixs issue with command 
namespace Aio {
class Command;
}

namespace Aio {

class Device
{
    public:
    static Device* CreateDevice(Context* context);
    virtual bool init() = 0;

    BufferHandle CreateUniformBufferHandle();
    BufferHandle CreateStorageBufferHandle();
    TextureHandle CreateTextureHandle();

    Command* pCommand;

    private:
    bool _started; // TODO: is this even used at all?

    std::vector<BufferHandle> _uniformBufferHandles;
    std::vector<BufferHandle> _storageBufferHandles;
    std::vector<TextureHandle> _textureHandles; 
};
} // End Aio namespace
#endif