#include <Arduino.h>
// ������ӻ����͵Ĳ����ַ���
#define testComHost "Hello slave"
// �ӻ��������ظ��Ĳ����ַ���
#define testComSlave "Hello host"
// ���ڱ�����
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
			// �ҵ������ַ���
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
