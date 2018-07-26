#pragma once

#include <Windows.h>
#include <fstream>
#include <vector>

class File
{
public:
	static std::vector<byte> GetAllBytes(const char *path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		if (!file)
			return std::vector<byte>(0);

		auto progSize = file.tellg();

		if (progSize > 0)
		{
			std::vector<byte> data((size_t)progSize);
			file.seekg(0, std::ios::beg);
			file.read((char*)&data[0], progSize);
			return data;
		}

		return std::vector<byte>();
	}

	static std::size_t WriteAllBytes(const byte *bytes, std::size_t size, const char *path)
	{
		std::ofstream file(path, std::ios::binary | std::ios::app | std::ios::beg);

		if (!file)
			return 0;

		file.write((const char*)bytes, size);

		return size;
	}
};