#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

// 调试标志
#define DEBUG

#ifndef uchr
#define uchr unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ull
#define ull unsigned long long
#endif

// 主机向从机发送的测试字符串
#define testComHost "Hello slave"
// 从机向主机回复的测试字符串
#define testComSlave "Hello host"
// 串口比特率
#define baudRate 115200L

#endif // __COMMON_DEF_H__