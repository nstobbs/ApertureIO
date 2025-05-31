#ifndef H_COMMAND
#define H_COMMAND

#include "Device.hpp"
#include "Context.hpp"
#include "RenderContext.hpp"

namespace Aio {

class Command
{
    public:
        static Command* CreateCommand(Device* device, Context* context);

        virtual void StartCommand(RenderContext& renderContext) = 0;
        virtual void EndCommand(RenderContext& renderContext) = 0;
        virtual void Draw(RenderContext& renderContext) = 0;
        virtual void DrawInstance(RenderContext& renderContext) = 0;
        virtual void DispatchCompute(RenderContext& renderContext) = 0;
        virtual void Clear(RenderContext& renderContext) = 0; //TODO maybe move this into framebuffer??
};

};
#endif