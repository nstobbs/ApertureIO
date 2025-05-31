#include "FileIO.hpp"
#include "Logger.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace Aio {

std::string FileIO::ReadSourceFile(const std::string& filepath)
{
    Logger::LogInfo("Reading Source File: " + filepath);
    std::ifstream file(filepath, std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        Logger::LogError("Failed to Read Source File: " + filepath);
        throw std::runtime_error("EXITING");
    };

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
};

std::string FileIO::SplitOutShader(std::string& sourceCode, SourceFileType shaderType)
{   
    // TODO Clean up this whole function
    std::stringstream output[3]; // TODO Set a Global For Number of ShaderTypes 
    std::string result;
    std::istringstream stream(sourceCode);
    std::string line;

    SourceFileType currentType = NoneShader;

    while(std::getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {   
            if(line.find("Vertex") != std::string::npos)
            {
                currentType = VertexShader;

            } else if (line.find("Fragment") != std::string::npos)
            {
                currentType = FragmentShader;

            } else if (line.find("Compute") != std::string::npos)
            {

                currentType = ComputeShader;

            };
        } else {
            if (currentType != NoneShader)
            {
                output[static_cast<int>(currentType)] << line << "\n";
            }
        }
    };

    result = output[static_cast<int>(shaderType)].str();

    return result;
};

};