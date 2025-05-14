#ifndef H_DEVICE
#define H_DEVICE

#include "Context.hpp"

namespace Aio {

class Device
{
    public:
    static Device* CreateDevice(Context* context);
    virtual bool init() = 0;

    private:
    bool _started;
};
} // End Aio namespace
#endif