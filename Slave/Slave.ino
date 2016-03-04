#include "../CommonDef.h"
#include "PID/PID.h"
#include "SlaveDef.h"

String inChar = "";
float r1, r2, kw = 18;
PID pid1(kp, ki, kd), pid2(kp, ki, kd);
float offset = 0;
int count_right = 0;
int count_left = 0;
float speeds, speeds_filter, positions;
int Speed_need = 0; int Turn_need = 0;
float diff_speeds, diff_speeds_all;
unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
unsigned long timer;
int label = 0;
float a[3], w[3], Angle[3], T;
short sAccelerat[3], sAngleVelocity[3], sAngle[3], sT;

unsigned long timestart;

int stickPos;
// 右起 1位为A 0位为B
uchr codeState;

// 初始化IO口
void initPin();
// 初始化串口
void initSerial();
// LED闪烁
void ledBlink();
// 处理A线变化
void codeA();
// 处理B线变化
void codeB();

void setup()
{
	initSerial();
	initPin();
	timestart = millis();
}

unsigned long timer3;
int switchone = 0;
int stick = 0;
int start = 0;
int getin = 0;

void loop()
{
	if (sign == 0) return;//sign为数据更新标志，每隔10ms更新一次，也就是说以下代码每隔10ms控制一次。
	if (label == 1)
	{
		if ((Angle[0] > 1 && r1 < 0 && w[0] > 2) || getin == 1)
		{
			getin = 1;
			switchone = 1;
			if (Angle[0] > offset)
			{
				label = 0;
				delay(4000);
				getin = 0;
				while (Angle[0] > 16)
				{
					delay(100);
				}
				timer3 = millis();
			}
			//attachInterrupt(22, Code_right, FALLING);attachInterrupt(26, Code_left, FALLING);
		}
	}
	sign = 0;
	Angle[0] = Angle[0];
	//kd = (float)analogRead(0)/1024*200;
	r1 = pid1.Update(Angle[0], Angle[0]);
	r1 = r1 + 40 * (r1 / abs(r1)) + w[0] * kw;//PID1、PID2函数就是第四节的PID函数，为了区分左右轮，所以分成两个。
	r2 = pid2.Update(Angle[0], Angle[0]);
	r2 = r2 + 40 * (r2 / abs(r2)) + w[0] * kw;
	///////////////////////////////////////////////////////////////////////////
	speeds = (count_left + count_right) * 0.5;
	diff_speeds = count_left - count_right;
	diff_speeds_all += diff_speeds;
	speeds_filter *= 0.85; //一阶互补滤波
	speeds_filter += speeds * 0.15;
	positions += speeds_filter;
	positions += Speed_need;
	positions = constrain(positions, -2300, 2300);//抗积分饱和 
	////////////////////
	r1 = r1 + 3.1 * speeds_filter + 0.06 * positions;
	r2 = r2 + 3.1 * speeds_filter + 0.06 * positions;
	//////////////////////////////////////////////////////////
	r2 = r2 + Turn_need;
	r1 = r1 - Turn_need;
	if (abs(r1) >= 255)
	{
		digitalWrite(led, HIGH);
	}
	else
	{
		digitalWrite(led, LOW);
	}
	if (switchone == 0)
	{
	}
	count_left = 0;
	count_right = 0;
	//delay(100);
}

void serialEvent1()
{
	while (Serial1.available())
	{ //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code 

		Re_buf[counter] = (unsigned char)Serial1.read();
		if (counter == 0 && Re_buf[0] != 0x55) return; //第0号数据不是帧头              
		counter++;
		if (counter == 11) //接收到11个数据
		{
			counter = 0; //重新赋值，准备下一帧数据的接收 
			switch (Re_buf[1])
			{
			case 0x51:
				a[0] = float(short(Re_buf[3] << 8 | Re_buf[2])) / 32768 * 16;
				a[1] = float(short(Re_buf[5] << 8 | Re_buf[4])) / 32768 * 16;
				a[2] = float(short(Re_buf[7] << 8 | Re_buf[6])) / 32768 * 16;
				break;
			case 0x52:
				w[0] = float(short(Re_buf[3] << 8 | Re_buf[2])) / 32768 * 250;
				w[1] = float(short(Re_buf[5] << 8 | Re_buf[4])) / 32768 * 250;
				w[2] = float(short(Re_buf[7] << 8 | Re_buf[6])) / 32768 * 250;
				break;
			case 0x53:
				Angle[0] = float(short(Re_buf[3] << 8 | Re_buf[2])) / 32768 * 180;
				Angle[1] = float(short(Re_buf[5] << 8 | Re_buf[4])) / 32768 * 180;
				Angle[2] = float(short(Re_buf[7] << 8 | Re_buf[6])) / 32768 * 180;
				T = float(short(Re_buf[9] << 8 | Re_buf[8]));///340.0+36.25   
				sign = 1;
				break;
			}
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
	Serial2.begin(baudRate);
	Serial1.begin(baudRate);
	while (!Serial1.available());
	for (int i = 0; i < 3;++i)
	{
		String tmp = Serial1.readString();
		if(strstr(tmp.c_str(), testComHost) != NULL)
		{
			// 找到握手字符串
			Serial1.println(testComSlave);
		}
	}
	ledBlink();
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
		codeState ^= 2;
		codeState ^= 1;
	}
}
