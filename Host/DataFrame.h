#pragma once

/*
����һ֡���ݣ�����dataΪ���ݣ�frameΪ����֡,countΪ�����ֽ���
������ԭ�ز��������255�ֽ�
*/
void makeDataFrame(void* data, void* frame, size_t count);

// У��һ֡���ݣ��Ϸ�����true�����򷵻�false
bool checkDataFrame(void* frame);

// ����һ֡���ݣ����صõ������ݸ���
template<typename DataType>
size_t decodeDataFrame(DataType* data, void* frame);