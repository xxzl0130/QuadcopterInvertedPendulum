#ifndef __DATA_FRAME_H__
#define __DATA_FRAME_H__

#include <stdint.h>

/*
构造一帧数据，参数data为数据，frame为数据帧,count为数据字节数。
不允许原地操作，最大255字节。
返回帧总字节数。
*/
uint8_t makeDataFrame(const void* data, uint8_t* frame, uint8_t count);

// 校验一帧数据，合法返回true，否则返回false
bool checkDataFrame(const uint8_t* frame);

// 解码一帧数据，返回得到的数据个数
uint8_t decodeDataFrame(float* data, const uint8_t* frame);

/*
从一段数据中找到一个完整的帧
参数为数据指针和数据字节数
返回指向帧头部的指针
*/
uint8_t* findDataFrame(uint8_t* data, uint8_t size);

#endif //__DATA_FRAME_H__