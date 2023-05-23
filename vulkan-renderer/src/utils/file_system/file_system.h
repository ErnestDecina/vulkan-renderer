#pragma once

#include <fstream>
#include <vector>

class FileSystem
{
public:
	static std::vector<char> readFile(const std::string&);
};