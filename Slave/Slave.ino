#include "CommonDef.h"
#include "PID.h"
#include "SlaveDef.h"
#include "DataFrame.h"
#ifdef __arm__
#include "DueTimer/DueTimer.h"
#else
#include "MsTimer2/MsTimer2.h"
#endif

// 缓冲区
uint8_t buf[MAX_BUF_SIZE];
// [0]x [1]y [2]z [3]FrameCount
float posFromHost[4];
float r1, r2, kw = 18;
PID pid1(kp, ki, kd), pid2(kp, ki, kd);
// 控制工作标志
bool workFlag = 0;
#ifdef __arm__
// 计时器
DueTimer *myTimer;
#endif
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
// led闪烁
void ledBlink();
// 串口事件函数
#ifdef DEBUG
void serialEvent();
#else
void serialEvent1();
#endif

void setup()
{
	initPin();
	ledBlink();
	initSerial();
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

#ifdef DEBUG
void serialEvent()
#else
void serialEvent1()
#endif
{
	static uint8_t count = 0;
	while(comSer.available())
	{
		buf[count] = comSer.read();
		if (buf[count] == static_cast<uint8_t>(0xa5) &&
			(buf[++count] = comSer.read()) == static_cast<uint8_t>(0x5a))
		{
			count = 0;
			buf[count++] = 0xa5;
			buf[count++] = 0x5a;
			buf[count++] = comSer.read();
			for (int i = 0; i <= buf[2];++i)
			{
				buf[count++] = comSer.read();
			}
			buf[count++] = comSer.read();
			if(checkDataFrame(buf))
			{
				decodeDataFrame(posFromHost, buf);
				workFlag = true;
			}
#ifdef  DEBUG
			for (int i = 0; i < 4; ++i)
			{
				comSer.print(posFromHost[0]);
				comSer.print(" ");
			}
			comSer.print("\n");
#endif
			return;
		}
		else
		{
			++count;
		}
	}
}

void initPin()
{
	pinMode(led, OUTPUT);
	digitalWrite(led, LOW);
	pinMode(CodePinA, INPUT_PULLUP);
	pinMode(CodePinB, INPUT_PULLUP);
#ifdef __arm__ //Due和Mega有不同的中断函数
	attachInterrupt(CodePinA, codeA, CHANGE);
	attachInterrupt(CodePinB, codeB, CHANGE);
#else
	attachInterrupt(0, codeA, CHANGE);
	attachInterrupt(1, codeB, CHANGE);
#endif
}

void initSerial()
{
	comSer.begin(gengeralBaudRate);
	/*while (!comSer.available());
	for (int i = 0; i < 3; ++i)
	{
		String tmp = comSer.readStringUntil('\n');
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{
			// 找到握手字符串
			comSer.println(testComSlave);
			break;
		}
	}*/
}

void initTimer()
{
#ifdef __arm__
	myTimer = new DueTimer(Timer.getAvailable());
	myTimer->attachInterrupt(doTimer);
	myTimer->setFrequency(workFrequency);
	myTimer->start();
#else
	MsTimer2::set(1000L / workFrequency, doTimer);
	MsTimer2::start();
#endif
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
