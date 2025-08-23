#include "ApertureIO/Command.hpp"
#include "ApertureIO/VulkanCommand.hpp"

namespace Aio {

UniquePtr<Command> Command::CreateCommand(Context* context, Device* device)
{
    auto API = context->getRendererAPI();
    switch (API)
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanCommand>(context, device);  
    };
};

}; // End of Aio namespace