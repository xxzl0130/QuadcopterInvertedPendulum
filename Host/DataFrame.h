#pragma once

/*
构造一帧数据，参数data为数据，frame为数据帧,count为数据字节数
不允许原地操作，最大255字节
*/
void makeDataFrame(void* data, void* frame, size_t count);

// 校验一帧数据，合法返回true，否则返回false
bool checkDataFrame(void* frame);

// 解码一帧数据，返回得到的数据个数
template<typename DataType>
size_t decodeDataFrame(DataType* data, void* frame);