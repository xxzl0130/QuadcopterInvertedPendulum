#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include <Arduino.h>

#define kp 20.0
#define ki 0.0001
#define kd 0.0

// 板载LED
#define led 13
// 编码器接口A
#define CodePinA	22
// 编码器接口B
#define CodePinB	23

// 缓冲区最大大小(256B)
#define MAX_BUF_SIZE (256)

// 控制频率(Hz)
#define workFrequency 100

#endif //__SLAVE_DEF_H__