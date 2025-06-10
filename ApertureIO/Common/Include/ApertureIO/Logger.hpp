#ifndef H_LOGGER
#define H_LOGGER

#include <string>
#include <iostream>

namespace Aio {

class Logger
{
    public:
    static void LogError(std::string text);
    static void LogWarn(std::string text);
    static void LogInfo(std::string text);

    private:
    static void setPrintColour(int colour);
    static void resetPrintColour();
    /* TODO: Create an file and add logs and time stamp them.*/
};

};

#endif