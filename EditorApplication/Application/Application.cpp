#include "Application/Application.hpp"


namespace Aio
{

Application::Application(ApplicationSettings settings) : _settings(settings), _QApplication(settings.argc, settings.argv){
    /* Init Application */
    _context = Context::CreateContext();
    _context->setRendererAPI(RendererAPI::eVulkan);
    _device = Device::CreateDevice(_context.get());
}

};