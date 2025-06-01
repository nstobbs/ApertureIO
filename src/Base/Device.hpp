#ifndef H_DEVICE
#define H_DEVICE

#include "Context.hpp"
#include "Command.hpp"

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

    Command* pCommand;

    private:
    bool _started;
};
} // End Aio namespace
#endif