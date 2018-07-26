#pragma once

#include <string>
#include <vector>

template<typename T>
std::vector<T> Xor(const T *values, std::size_t size, const char *key)
{
	std::vector<T> ret(values, values + size);
	size_t keyLen = strlen(key);

	for (size_t i = 0; i < size; i++)
		ret[i] = ret[i] ^ (key[(i + 1) % keyLen] ^ key[i % keyLen]);

	return ret;
}

