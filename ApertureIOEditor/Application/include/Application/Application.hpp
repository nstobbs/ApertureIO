#pragma once

#include "ApertureIO/Pointers.hpp"

#include <QApplication>
#include <string>

namespace Aio::Application {

struct ApplicationSettings
{
    std::string name = {"ApertureIOEditor"};
    uint32_t minorVerion = {1};
    uint32_t majorVerion = {0};
    bool headless = {false};
    /* Set FilePath to RenderGraph to Launch with the Application */
    std::string openRenderGraph = {"None"};
};

class Application
{
public:
    Application(ApplicationSettings settings);
    ~Application();
private:
    UniquePtr<QApplication> _QApp;
    ApplicationSettings _settings;
};

};