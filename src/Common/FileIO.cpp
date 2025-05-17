#include "FileIO.hpp"
#include "Logger.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace Aio {

std::vector<char> FileIO::ReadSourceFile(const std::string& filepath)
{
    Logger::LogInfo("Reading Source File: " + filepath);
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        Logger::LogError("Failed to Read Source File: " + filepath);
        throw std::runtime_error("EXITING");
    };

    size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    
    return buffer;
};

std::vector<std::string> FileIO::SplitOutShader(std::vector<char>& sourceCode, std::string shaderType)
{
    std::vector<std::string> blank;
    return blank;
};

};