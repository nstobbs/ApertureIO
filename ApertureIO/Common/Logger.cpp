#include "ApertureIO/Logger.hpp"

#include <ctime>

namespace Aio {

std::string Logger::GetCurrentTime()
{
    std::time_t timestamp = std::time(NULL);
    struct tm datetime = *localtime(&timestamp);
    char timeChar[50];
    strftime(timeChar, 50, "%H:%M:%S", &datetime);
    std::string currentTime(timeChar);

    currentTime = "{" + currentTime +"} ";
    return currentTime;
};

void Logger::LogError(std::string text)
{
    std::cout << GetCurrentTime();
    setPrintColour(31);
    std::cout << "Error: ";
    resetPrintColour();
    std::cout << text << "\n";
    throw std::runtime_error("Exiting..");
};

void Logger::LogWarn(std::string text)
{
    std::cout << GetCurrentTime();
    setPrintColour(33);
    std::cout << "Warning: ";
    resetPrintColour();
    std::cout << text << "\n";
};

void Logger::LogInfo(std::string text)
{
    std::cout << GetCurrentTime();
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