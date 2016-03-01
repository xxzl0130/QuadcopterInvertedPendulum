#include "DataFrame.h"
#include <cstdint>
#include <memory.h>

size_t makeDataFrame(const void* data, uint8_t* frame, size_t count)
{
	auto *scr = static_cast<const uint8_t*>(data);
	auto *dst = static_cast<uint8_t*>(frame);
	if (count > 0xff)
		return 0;
	// ��ʼ��־
	dst[0] = 0xa5;
	dst[1] = 0x5a;
	// �����ֽ���
	dst[2] = count + 1;
	// �ѽ�����
	dst[3] = 0xa1;
	// ��������
	memcpy(dst + 4, scr, count);
	// У��
	dst[count + 4] = 0;
	for (auto i = 0u; i <= count + 1;++i)
		dst[count + 4] += dst[i + 2];
	// �������
	dst[count + 5] = 0xaa;
	return (count + 6);
}

bool checkDataFrame(const uint8_t* frame)
{
	auto *data = static_cast<const uint8_t*>(frame);
	uint8_t sum = 0;
	// �����ʼ���
	if (data[0] != static_cast<uint8_t>(0xa5) || data[1] != static_cast<uint8_t>(0x5a))
	{
		return false;
	}
	// У��
	for (auto i = 0u;i <= data[2];++i)
	{
		sum += data[i + 2];
	}
	// ���У��ֵ�ͽ������
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
	auto frameData = static_cast<const uint8_t*>(frame), ptr = frameData + 3;
	count = (frameData[2] - 1) / sizeof(DataType);
	for (auto i = 0u; i < count; ++i, ptr += sizeof(DataType))
	{
		data[i] = *reinterpret_cast<DataType*>(ptr);
	}
	return count;
}

uint8_t* findDataFrame(uint8_t* data, size_t size)
{
	for (auto i = 0; i < size;++i)
	{
		if(data[i] == static_cast<uint8_t>(0xa5) && checkDataFrame(data + i))
		{
			return (data + i);
		}
	}
	return nullptr;
}