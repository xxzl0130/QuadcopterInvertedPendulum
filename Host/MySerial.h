#pragma once
#include "DataFrame.h"
#include "serial/serial.h"
#include <stdint.h>
#include <string>

/*
��Serial����м̳У����DataFrame����
*/
class MySerial :public serial::Serial
{
public:
	/*
	��������DataFrame����ʽ���͡�
	���ط��͵����ֽ�����
	*/
	size_t sendDataFrame(uint8_t *data, size_t size);
	size_t sendDataFrame(std::string &data);
	/*
	����һ֡DataFrame����֡��ǰ�����ݽ���������
	���ؽ��յ����ֽ�����
	*/
	size_t receiveDataFrame(uint8_t *buf);
};