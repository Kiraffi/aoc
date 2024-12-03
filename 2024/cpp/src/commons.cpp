
#include "commons.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string readInputFile(const std::string& filename)
{
    std::string result;
    std::ifstream file(filename.c_str(), std::ios::binary);
    if(!file.is_open())
    {
        printf("Failed to open file\n");
        return result;
    }
    result.assign(std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
    return result;
}
