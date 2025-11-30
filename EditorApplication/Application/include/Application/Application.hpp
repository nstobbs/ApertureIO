#pragma once

#include "ApertureIO/Pointers.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"

#include <QApplication>
#include <string>

namespace Aio {

struct ApplicationSettings
{
    ApplicationSettings() = default;
    ApplicationSettings(int count, char** args) : argc(count), argv(args) {}
    std::string name = {"ApertureIOEditor"};
    uint32_t minorVerion = {0};
    uint32_t majorVerion = {1};
    bool isHeadless = {false};
    std::string openRenderGraph = {"None"}; /* Set FilePath to RenderGraph to Launch with the Application */

    /* Args */
    int argc = {0};
    char** argv = {nullptr};
};

class Application
{
public:
    Application(ApplicationSettings settings);
    ~Application() = default;

private:
    QApplication _QApplication;
    ApplicationSettings _settings;
    
    UniquePtr<Context>  _context;
    UniquePtr<Device>  _device;
};

};