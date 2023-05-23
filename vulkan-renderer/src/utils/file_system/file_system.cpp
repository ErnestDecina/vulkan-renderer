#include "file_system.h"
#include <iostream>

/**
*   readFile
*   desc:
*       Reads contents of a file from file system
*/
std::vector<char> FileSystem::readFile(const std::string& file_name)
{
    std::ifstream file(file_name, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    } // End if 

    size_t file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();

    return buffer;
} // End if 
