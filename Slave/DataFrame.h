#ifndef __DATA_FRAME_H__
#define __DATA_FRAME_H__

#include <stdint.h>

/*
����һ֡���ݣ�����dataΪ���ݣ�frameΪ����֡,countΪ�����ֽ�����
������ԭ�ز��������255�ֽڡ�
����֡���ֽ�����
*/
uint8_t makeDataFrame(const void* data, uint8_t* frame, uint8_t count);

// У��һ֡���ݣ��Ϸ�����true�����򷵻�false
bool checkDataFrame(const uint8_t* frame);

// ����һ֡���ݣ����صõ������ݸ���
uint8_t decodeDataFrame(float* data, const uint8_t* frame);

/*
��һ���������ҵ�һ��������֡
����Ϊ����ָ��������ֽ���
����ָ��֡ͷ����ָ��
*/
uint8_t* findDataFrame(uint8_t* data, uint8_t size);

#endif //__DATA_FRAME_H__