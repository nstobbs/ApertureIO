#include "Logger.hpp"

namespace Aio {

void Logger::LogError(std::string text)
{ 
    std::cout << "[.:Log:.] ";
    setPrintColour(31);
    std::cout << "Error: ";
    resetPrintColour();
    std::cout << text << "\n";
};

void Logger::LogWarn(std::string text)
{
    std::cout << "[.:Log:.] ";
    setPrintColour(33);
    std::cout << "Warning: ";
    resetPrintColour();
    std::cout << text << "\n";
};

void Logger::LogInfo(std::string text)
{
    std::cout << "[.:Log:.] ";
    setPrintColour(32);
    std::cout << "Info: ";
    resetPrintColour();
    std::cout << text << "\n";
};

void Logger::setPrintColour(int colour)
{
    // Red 31
    // Green 32
    // Yellow 33
    std::cout << "\033[" << colour << "m";
};


void Logger::resetPrintColour()
{
    std::cout << "\033[0m";
};

};