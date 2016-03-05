#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

// 调试标志
#define DEBUG

typedef unsigned char uchr;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ull;

// 主机向从机发送的测试字符串
#define testComHost "Hello slave"
// 从机向主机回复的测试字符串
#define testComSlave "Hello host"
// 串口比特率
#define gengeralBaudRate 115200L

#endif // __COMMON_DEF_H__