#include "ApertureIO/Command.hpp"
#include "ApertureIO/VulkanCommand.hpp"

namespace Aio {

Command* Command::CreateCommand(Context* context, Device* device)
{
    auto API = context->getRendererAPI();
    switch (API)
    {
        case RendererAPI::eVulkan:
            return new VulkanCommand(context, device);
    };
};

}; // End of Aio namespace