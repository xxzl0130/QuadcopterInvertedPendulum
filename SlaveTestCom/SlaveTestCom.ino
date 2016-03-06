#include <Arduino.h>
// 主机向从机发送的测试字符串
#define testComHost "Hello slave"
// 从机向主机回复的测试字符串
#define testComSlave "Hello host"
// 串口比特率
#define gengeralBaudRate 115200L
#define comSer Serial
void setup() {
	comSer.begin(gengeralBaudRate);
	/*while (!comSer.available());
	for (int i = 0; i < 3; ++i)
	{
		String tmp = comSer.readString();
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{
			// 找到握手字符串
			comSer.println(testComSlave);
			break;
		}
	}*/
}

void loop() {
	String tmp;
	if(Serial.available())
	{
		tmp = Serial.readStringUntil('\n');
		Serial.println(tmp);
	}
}
