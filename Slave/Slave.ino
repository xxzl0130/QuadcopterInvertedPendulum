#ifndef __arm__
#define __arm__
#endif
#include "CommonDef.h"
#include "PID.h"
#include "SlaveDef.h"
#include "DataFrame.h"
#include "DueTimer/DueTimer.h"

// 缓冲区
uint8_t buf[MAX_BUF_SIZE];
// [0]x [1]y [2]z [3]FrameCount
float posFromHost[4];
float r1, r2, kw = 18;
PID pid1(kp, ki, kd), pid2(kp, ki, kd);
// 控制工作标志
bool workFlag = 0;
// 计时器
DueTimer *myTimer;
// 由旋转编码器计算的摆的位置
int stickPos;
// 右起 1位为A 0位为B
uchr codeState;

// 初始化IO口
void initPin();
// 初始化串口
void initSerial();
// 初始化定时器
void initTimer();
// 处理A线变化
void codeA();
// 处理B线变化
void codeB();
// 定时器回调函数
void doTimer();

void setup()
{
	initSerial();
	initPin();
	initTimer();
}

void loop()
{
	if (!workFlag) 
		return;
	//workFlag为数据更新标志，每隔10ms更新一次，也就是说以下代码每隔10ms控制一次。
	workFlag = false;
	r1 = pid1.update(stickPos, stickPos);
	r2 = pid2.update(stickPos, stickPos);
}

void serialEvent1()
{
	static uint8_t count = 0;
	while(Serial1.available())
	{
		buf[count] = Serial1.read();
		if (buf[count] == static_cast<uint8_t>(0xa5) &&
			(buf[++count] = Serial1.read()) == static_cast<uint8_t>(0x5a))
		{
			count = 0;
			buf[count++] = 0xa5;
			buf[count++] = 0x5a;
			buf[count++] = Serial1.read();
			for (int i = 0; i <= buf[2];++i)
			{
				buf[count++] = Serial1.read();
			}
			buf[count++] = Serial1.read();
			if(checkDataFrame(buf))
			{
				decodeDataFrame(posFromHost, buf);
				workFlag = true;
			}
			return;
		}
	}
}

void initPin()
{
	pinMode(led, OUTPUT);
	pinMode(CodePinA, INPUT_PULLUP);
	pinMode(CodePinB, INPUT_PULLUP);
	attachInterrupt(CodePinA, codeA, CHANGE);
	attachInterrupt(CodePinB, codeB, CHANGE);
}

void initSerial()
{
	Serial2.begin(gengeralBaudRate);
	Serial1.begin(gengeralBaudRate);
	while (!Serial1.available());
	for (int i = 0; i < 3;++i)
	{
		String tmp = Serial1.readString();
		if(strstr(tmp.c_str(), testComHost) != NULL)
		{
			// 找到握手字符串
			Serial1.println(testComSlave);
			break;
		}
	}
}

void initTimer()
{
	myTimer = new DueTimer(Timer.getAvailable());
	myTimer->attachInterrupt(doTimer);
	myTimer->setFrequency(workFrequency);
	myTimer->start();
}

void ledBlink()
{
	for (int i = 0; i < 3;++i)
	{
		digitalWrite(led, HIGH);
		delay(200);
		digitalWrite(led, LOW);
		delay(200);
	}
}

void codeA()
{
	if(digitalRead(CodePinA) != (codeState >> 1))
	{
		switch(codeState)
		{
		case 3:case 0:
			++stickPos;
			break;
		case 1:case 2:
			--stickPos;
			break;
		default:
			codeState = 0;
			break;
		}
		// A位改变
		codeState ^= 2;
	}
}

void codeB()
{
	if (digitalRead(CodePinB) != codeState & 1)
	{
		switch (codeState)
		{
		case 3:case 0:
			--stickPos;
			break;
		case 1:case 2:
			++stickPos;
			break;
		default:
			codeState = 0;
			break;
		}
		// B位改变
		codeState ^= 1;
	}
}

void doTimer()
{
	workFlag ^= 1;
}
