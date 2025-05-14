#ifndef H_COMMAND
#define H_COMMAND

#include "Device.hpp"
#include "Context.hpp"

namespace Aio {

class Command
{
    public:
        static Command* CreateCommand(Device* device, Context* context);

        virtual void StartCommand(Device* pDevice) = 0;
        virtual void EndCommand(Device* pDevice) = 0;
        virtual void Draw(Device* pDevice) = 0;
        virtual void DrawInstance(Device* pDevice) = 0;
        virtual void DispatchCompute(Device* pDevice) = 0;
        virtual void Clear(Device* pDevice) = 0; //TODO maybe move this into framebuffer??
};

};
#endif