#include<Servo.h>
Servo myservo;
String inChar = "";
float kp = 20, ki = 0.0000, kd = 0, r1, r2, kw = 18;
float offset = 0;
int count_right = 0;
int count_left = 0;
float speeds, speeds_filter, positions;
int Speed_need = 0; int Turn_need = 0;
float diff_speeds, diff_speeds_all;
unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
int M11 = 12;
int M12 = 11;
int M21 = 10;
int M22 = 9;
#define led 13
#define kick1 2
#define kick2 3
unsigned long timer;
int label = 0;
float a[3], w[3], Angle[3], T;
short sAccelerat[3], sAngleVelocity[3], sAngle[3], sT;
void Code_right() { if (r1 >= 0) { count_right += 1; } else { count_right -= 1; } }//if only +,can't stand up 编码器码盘计数加一 
void Code_left() { if (r2 >= 0) { count_left += 1; } else { count_left -= 1; } }// 编码器码盘计数加一 
void kick(int time)
{
	digitalWrite(kick1, HIGH);
	delay(time);
	digitalWrite(kick1, LOW);
}
unsigned long timestart;
void setup() {
	// initialize serial:
	Serial2.begin(115200);
	Serial1.begin(115200);
	pinMode(led, OUTPUT);
	pinMode(M11, OUTPUT); analogWrite(M11, 0);
	pinMode(M12, OUTPUT); analogWrite(M12, 0);
	pinMode(M21, OUTPUT); analogWrite(M21, 0);
	pinMode(M22, OUTPUT); analogWrite(M22, 0);
	pinMode(22, INPUT);
	pinMode(26, INPUT);
	pinMode(kick1, OUTPUT);
	pinMode(kick2, OUTPUT);
	digitalWrite(kick1, LOW);
	digitalWrite(kick2, LOW);
	attachInterrupt(22, Code_right, FALLING); attachInterrupt(26, Code_left, FALLING);
	myservo.attach(52);
	delay(5000);
	myservo.write(40);
	delay(2000);
	while (Angle[0]>16)
	{
		delay(100);
	}
	timestart = millis();
}
void SetMotor(float v1, float v2)
{
	if (v1>255) { v1 = 255; analogWrite(M11, 0); analogWrite(M12, v1); }
	else if (v1>0) { analogWrite(M11, 0); analogWrite(M12, v1); }
	else if (v1>-255) { analogWrite(M12, 0); analogWrite(M11, -v1); }
	else { v1 = -255; analogWrite(M12, 0); analogWrite(M11, -v1); }

	if (v2>255) { v2 = 255; analogWrite(M21, 0); analogWrite(M22, v2); }
	else if (v2>0) { analogWrite(M21, 0); analogWrite(M22, v2); }
	else if (v2>-255) { analogWrite(M22, 0); analogWrite(M21, -v2); }
	else { v2 = -255; analogWrite(M22, 0); analogWrite(M21, -v2); }
}

float PID1(float e, float kp, float ki, float kd)
{
	static float es = 0, sum = 0;
	float r;
	sum += e;
	r = kp*e + ki*sum + kd*(e - es);
	es = e;
	return r;
}
float PID2(float e, float kp, float ki, float kd)
{
	static float es = 0, sum = 0;
	float r;
	sum += e;
	r = kp*e + ki*sum + kd*(e - es);
	es = e;
	return r;
}
int label3 = 0;
unsigned long timer3;
int switchone = 0;
int stick = 0;
int start = 0;
int getin = 0;
void loop() {
	if (start == 0 && millis() - timestart >= 3000)
	{
		myservo.write(160);
		start = 1;
	}

	if ((label3 == 1) && (millis() - timer3 >= 200))
	{

		count_right = 0;
		count_left = 0;
		speeds = 0;
		speeds_filter = 0;
		positions = 0;
		Speed_need = 0;
		Turn_need = 0;
		diff_speeds = 0;
		diff_speeds_all = 0;
		analogWrite(M11, 0);
		analogWrite(M12, 0);
		analogWrite(M21, 0);
		analogWrite(M22, 0);
		attachInterrupt(22, Code_right, FALLING); attachInterrupt(26, Code_left, FALLING);
		switchone = 0;
		label3 = 0;
	}
	if (sign == 0) return;//sign为数据更新标志，每隔10ms更新一次，也就是说以下代码每隔10ms控制一次�?
	if (label == 1)
	{
		if ((Angle[0]>1 && r1<0 && w[0]>2) || getin == 1)
		{
			getin = 1;
			SetMotor(0, 0);
			switchone = 1;
			if (Angle[0]>offset)
			{
				digitalWrite(kick1, HIGH);
				delay(400);
				digitalWrite(kick1, LOW);
				label = 0;
				delay(4000);
				myservo.write(40);
				getin = 0;
				while (Angle[0]>16)
				{
					delay(100);
				}
				label3 = 1;
				timer3 = millis();
				stick = 1;
			}
			//attachInterrupt(22, Code_right, FALLING);attachInterrupt(26, Code_left, FALLING);
		}
	}
	if (stick == 1 && millis() - timer3 >= 3000)
	{
		myservo.write(160);
		stick = 0;
	}
	sign = 0;
	Angle[0] = Angle[0];
	//kd = (float)analogRead(0)/1024*200;
	r1 = PID1(Angle[0], kp, ki, kd);
	r1 = r1 + 40 * (r1 / abs(r1)) + w[0] * kw;//PID1、PID2函数就是第四节的PID函数，为了区分左右轮，所以分成两个�?
	r2 = PID2(Angle[0], kp, ki, kd);
	r2 = r2 + 40 * (r2 / abs(r2)) + w[0] * kw;
	///////////////////////////////////////////////////////////////////////////
	speeds = (count_left + count_right)*0.5;
	diff_speeds = count_left - count_right;
	diff_speeds_all += diff_speeds;
	speeds_filter *= 0.85;  //一阶互补滤�?
	speeds_filter += speeds*0.15;
	positions += speeds_filter;
	positions += Speed_need;
	positions = constrain(positions, -2300, 2300);//抗积分饱�?
												  ////////////////////
	r1 = r1 + 3.1*speeds_filter + 0.06*positions;
	r2 = r2 + 3.1*speeds_filter + 0.06*positions;
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
		SetMotor(r1, r2);//设置电机转速�?
	}
	//Serial2.print("angle x:");
	Serial2.print("S" + String(int(Angle[0])) + "E");//x
	Serial2.print("A" + String(int(w[0])) + "B");//y
												 //Serial2.print("A"+String(int(offset))+"B");
	Serial2.print("V" + String(int(count_left)) + "C");
	count_left = 0;
	count_right = 0;
	//delay(100);
}

void serialEvent1() {
	while (Serial1.available()) {        //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code 

		Re_buf[counter] = (unsigned char)Serial1.read();
		if (counter == 0 && Re_buf[0] != 0x55) return;      //�?号数据不是帧�?             
		counter++;
		if (counter == 11)             //接收�?1个数�?
		{
			counter = 0;               //重新赋值，准备下一帧数据的接收 
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
void serialEvent2()
{
	//String inChar="";
	String temp = "";
	while (Serial2.available()) {
		// get the new byte:
		inChar = inChar + (char)Serial2.read();
		delay(30);
	}
	// add it to the inputString:
	//////////////////if(inChar=="1")
	/////////{
	//analogWrite(M11,255);
	//analogWrite(M12,255);
	//analogWrite(M21,255);
	//analogWrite(M22,255);
	//detachInterrupt(22);
	//detachInterrupt(26);
	//digitalWrite(kick1,HIGH);
	// kick(200);


	// /////////////}
	switch (inChar[0]) {
	case 'a': {Speed_need = 200; Turn_need = 0; positions = 300; inChar = ""; }; break;//Go
	case 'b': {Speed_need = 20; Turn_need = -20; positions = 10; inChar = ""; }; break;//right
	case 'c': {Speed_need = 20; Turn_need = 20; positions = 10; inChar = ""; }; break;//left 
	case 'd': {Speed_need = 0; Turn_need = 0; positions = 0; inChar = ""; }; break;
		//default:Speed_need=0;Turn_need=0;positions=0;break;//stop
	}
	//////////////////////////////////////////////////             ////////////////////////////////////////////////////////////////////////////////////////
	//Serial2.println(inChar);
	for (int i = 0; inChar[i] != '\0'; i++)
	{
		if (inChar[i] == 'G')
		{
			i++;
			for (i; inChar[i] != '\0'; i++)
			{
				if (inChar[i] == 'H')
				{
					offset = temp.toInt();
					inChar = "";
					temp = "";
					label = 1;
					timer = millis();
					break;
				}
				temp = temp + inChar[i];

			}
			break;
		}
	}
	//Serial2.print("A"+temp+"B");


}



