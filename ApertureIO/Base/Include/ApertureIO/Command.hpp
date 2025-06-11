#pragma once

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"

namespace Aio {
    class Device;
    class RenderContext;
}

namespace Aio {

class Command
{
    public:
        static Command* CreateCommand(Context* context, Device* device);

        virtual void StartCommand(RenderContext& renderContext) = 0;
        //virtual void EndCommand(RenderContext& renderContext) = 0;
        virtual void Draw(RenderContext& renderContext) = 0;
        //virtual void DrawInstance(RenderContext& renderContext) = 0;
        //virtual void DispatchCompute(RenderContext& renderContext) = 0;
        //virtual void Clear(RenderContext& renderContext) = 0; //TODO maybe move this into framebuffer??
};

};