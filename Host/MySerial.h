#pragma once
#include "DataFrame.h"
#include "serial/serial.h"
#include <stdint.h>
#include <string>

/*
对Serial类进行继承，添加DataFrame操作
*/
class MySerial :public serial::Serial
{
public:
	/*
	将数据以DataFrame的形式发送。
	返回发送的总字节数。
	*/
	size_t sendDataFrame(uint8_t *data, size_t size);
	size_t sendDataFrame(std::string &data);
	/*
	接收一帧DataFrame，该帧以前的数据将被抛弃。
	返回接收的总字节数。
	*/
	size_t receiveDataFrame(uint8_t *buf);
};