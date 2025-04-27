#ifndef H_DEVICE
#define H_DEVICE

#include "Context.hpp"

namespace ApertureIO {

class Device
{
    public:
    static Device* CreateDevice(Context* context);
    virtual bool init() = 0;

    private:
    bool _started;
};
} // End ApertureIO namespace
#endif