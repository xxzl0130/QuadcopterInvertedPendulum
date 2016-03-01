#include "MySerial.h"
#include <ctime>
#include <string>
#include <memory.h>

size_t MySerial::sendDataFrame(uint8_t *data, size_t size)
{
	size_t frameSize;
	auto *frame = new uint8_t[size + 10];
	frameSize = makeDataFrame(data, frame, size);
	this->write(frame, frameSize);
	delete frame;
	return frameSize;
}

size_t MySerial::sendDataFrame(std::string& data)
{
	auto *frame = new uint8_t[data.size() + 10];
	memcpy_s(frame, data.size() + 10, data.c_str(), data.size());
	auto size = sendDataFrame(frame, data.size());
	delete frame;
	return size;
}

size_t MySerial::receiveDataFrame(uint8_t* buf)
{
	size_t size;
	auto time = clock();
	while (true)
	{
		// 超时退出
		if (clock() - time > CLOCKS_PER_SEC)
			return 0;
		// 一次读一字节
		this->read(buf, 1);
		// 找起始字节
		if (buf[0] == static_cast<uint8_t>(0xa5))
		{
			this->read(buf + 1, 1);
			if (buf[1] == static_cast<uint8_t>(0x5a))
			{
				this->read(buf + 2, 1);
				this->read(buf + 3, buf[2] + 1);
				size = buf[2] + 4;
				return size;
			}
		}
	}
}
