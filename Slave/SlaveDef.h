#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include <Arduino.h>

#define DEBUG

#define kp 20.0
#define ki 0.0001
#define kd 0.0

// 板载LED
#define led 13
// 编码器接口A
#define CodePinA	2
// 编码器接口B
#define CodePinB	3
// 使用2 3pin以兼容Mega/Due

// 缓冲区最大大小(256B)
#define MAX_BUF_SIZE (256)

// 控制频率(Hz)
#define workFrequency 100

#ifdef DEBUG
#define comSer Serial
#else
#define comSer Serial1
#endif

#endif //__SLAVE_DEF_H__