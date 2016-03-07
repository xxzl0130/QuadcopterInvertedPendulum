#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include <Arduino.h>

#define DEBUG

#define kp 20.0
#define ki 0.0001
#define kd 0.0

// ����LED
#define led 13
// �������ӿ�A
#define CodePinA	2
// �������ӿ�B
#define CodePinB	3
// ʹ��2 3pin�Լ���Mega/Due

// ����������С(256B)
#define MAX_BUF_SIZE (256)

// ����Ƶ��(Hz)
#define workFrequency 100

#ifdef DEBUG
#define comSer Serial
#else
#define comSer Serial1
#endif

#endif //__SLAVE_DEF_H__