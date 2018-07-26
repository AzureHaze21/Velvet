#pragma once

#include <Windows.h>

namespace BinUtils
{
	bool memCompare(const byte *src, const byte *dst, const byte *mask)
	{
		while (*mask)
		{
			if (*mask == 'x' && *src != *dst)
				return false;
			src++;
			dst++;
			mask++;
		}

		return true;
	}

	size_t sigScan(void *data, size_t size, const byte *sig, const byte *mask)
	{
		const byte *pData = (const byte*)data;

		for (DWORD i = 0; i < size; i++)
		{
			if (memCompare(pData + i, (const byte*)sig, mask))
				return i;
		}

		return 0;
	}
}