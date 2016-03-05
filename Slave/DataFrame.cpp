#include <Arduino.h>
#include "DataFrame.h"
#include <stdint.h>
#include <string.h>

size_t makeDataFrame(const void* data, uint8_t* frame, size_t count)
{
	const uint8_t* scr = static_cast<const uint8_t*>(data);
	uint8_t* dst = static_cast<uint8_t*>(frame);
	if (count > 0xff)
		return 0;
	// 起始标志
	dst[0] = 0xa5;
	dst[1] = 0x5a;
	// 数据字节数
	dst[2] = count + 1;
	// 已解算标记
	dst[3] = 0xa1;
	// 拷贝数据
	memcpy(dst + 4, scr, count);
	// 校验
	dst[count + 4] = 0;
	for (int i = 0u; i <= count + 1;++i)
		dst[count + 4] += dst[i + 2];
	// 结束标记
	dst[count + 5] = 0xaa;
	return (count + 6);
}

bool checkDataFrame(const uint8_t* frame)
{
	const uint8_t* data = static_cast<const uint8_t*>(frame);
	uint8_t sum = 0;
	// 检查起始标记
	if (data[0] != static_cast<uint8_t>(0xa5) || data[1] != static_cast<uint8_t>(0x5a))
	{
		return false;
	}
	// 校验
	for (int i = 0u;i <= data[2];++i)
	{
		sum += data[i + 2];
	}
	// 检查校验值和结束标记
	if (sum != data[data[2] + 3] || data[data[2] + 4] != static_cast<uint8_t>(0xaa))
	{
		return false;
	}
	return true;
}

template <typename DataType>
size_t decodeDataFrame(DataType* data, const uint8_t* frame)
{
	size_t count;
	const uint8_t *frameData = static_cast<const uint8_t*>(frame), *ptr = frameData + 3;
	count = (frameData[2] - 1) / sizeof(DataType);
	for (int i = 0u; i < count; ++i, ptr += sizeof(DataType))
	{
		data[i] = *reinterpret_cast<DataType*>(ptr);
	}
	return count;
}

uint8_t* findDataFrame(uint8_t* data, size_t size)
{
	for (int i = 0; i < size;++i)
	{
		if(data[i] == static_cast<uint8_t>(0xa5) && checkDataFrame(data + i))
		{
			return (data + i);
		}
	}
	return NULL;
}