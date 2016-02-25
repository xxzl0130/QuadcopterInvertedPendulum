#include "DataFrame.h"
#include <cstdint>
#include <memory.h>

void makeDataFrame(void* data, void* frame, size_t count)
{
	auto *scr = static_cast<int8_t*>(data), *dst = static_cast<int8_t*>(frame);
	if (count > 0xff)
		return;
	dst[0] = 0xa5;
	dst[1] = 0x5a;
	dst[2] = count;
	memcpy(dst + 3, scr, count);
	dst[count + 3] = 0;
	for (auto i = 0u;i <= count;++i)
		dst[count + 3] += dst[i + 2];
	dst[count + 4] = 0xaa;
}

bool checkDataFrame(void* frame)
{
	int8_t *data = static_cast<int8_t*>(frame), sum = 0;
	if (data[0] != static_cast<int8_t>(0xa5) || data[1] != static_cast<int8_t>(0x5a))
	{
		return false;
	}
	for (auto i = 0u;i <= data[2];++i)
	{
		sum += data[i + 2];
	}
	if (sum != data[data[2] + 3] || data[data[2] + 4] != static_cast<int8_t>(0xaa))
	{
		return false;
	}
	return true;
}

template <typename DataType>
size_t decodeDataFrame(DataType* data, void* frame)
{
	size_t count;
	auto frameData = static_cast<int8_t*>(frame);
	count = frameData[2] / sizeof(DataType);
	for (auto i = 0u, ptr = frameData + 3; i < count; ++i , ptr += sizeof(DataType))
	{
		data[i] = *reinterpret_cast<DataType*>(ptr);
	}
	return count;
}
