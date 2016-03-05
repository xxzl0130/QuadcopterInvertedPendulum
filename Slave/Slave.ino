#ifndef __arm__
#define __arm__
#endif
#include "CommonDef.h"
#include "PID.h"
#include "SlaveDef.h"
#include "DataFrame.h"
#include "DueTimer/DueTimer.h"

// ������
uint8_t buf[MAX_BUF_SIZE];
// [0]x [1]y [2]z [3]FrameCount
float posFromHost[4];
float r1, r2, kw = 18;
PID pid1(kp, ki, kd), pid2(kp, ki, kd);
// ���ƹ�����־
bool workFlag = 0;
// ��ʱ��
DueTimer *myTimer;
// ����ת����������İڵ�λ��
int stickPos;
// ���� 1λΪA 0λΪB
uchr codeState;

// ��ʼ��IO��
void initPin();
// ��ʼ������
void initSerial();
// ��ʼ����ʱ��
void initTimer();
// ����A�߱仯
void codeA();
// ����B�߱仯
void codeB();
// ��ʱ���ص�����
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
	//workFlagΪ���ݸ��±�־��ÿ��10ms����һ�Σ�Ҳ����˵���´���ÿ��10ms����һ�Ρ�
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
			// �ҵ������ַ���
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
		// Aλ�ı�
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
		// Bλ�ı�
		codeState ^= 1;
	}
}

void doTimer()
{
	workFlag ^= 1;
}
