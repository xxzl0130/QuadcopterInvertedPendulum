#pragma once

#include <stdint.h>

/*
����һ֡���ݣ�����dataΪ���ݣ�frameΪ����֡,countΪ�����ֽ�����
������ԭ�ز��������255�ֽڡ�
����֡���ֽ�����
*/
size_t makeDataFrame(const void* data, uint8_t* frame, size_t count);

// У��һ֡���ݣ��Ϸ�����true�����򷵻�false
bool checkDataFrame(const uint8_t* frame);

// ����һ֡���ݣ����صõ������ݸ���
template<typename DataType>
size_t decodeDataFrame(DataType* data, const uint8_t* frame);

/*
��һ���������ҵ�һ��������֡
����Ϊ����ָ��������ֽ���
����ָ��֡ͷ����ָ��
*/
uint8_t* findDataFrame(uint8_t* data,size_t size);