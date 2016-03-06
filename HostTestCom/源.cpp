#include<string>
#include<iostream>
#include <windows.h>
#include "serial/serial.h"
#include "../Slave/CommonDef.h"
#include "../host/DataFrame.h"
using namespace std;
using namespace serial;

char buf[1024];// ������
Serial slave;// �ӻ�ͨ�Ŵ���

union int16_8
{
	unsigned __int16 i16;
	unsigned __int8 i8[2];
};

/*
ͨ��main����������ʼ�����ڡ�
argv[1]������Ϊ�˿ڣ����������ɱ�׼�����ȡ
argv[2]������Ϊ�����ʣ���������ʹ��Ĭ�ϲ�����baudRate
��ȡ�˿ںͲ����ʺ����initSerial���н�һ����ʼ��
*/
void initSerialArg(int argc, char** argv);
/*
����portΪ�˿ڣ�����baudΪ�����ʣ�Ĭ��ΪbaurdRate��
ʹ��Serial���ṩ�ĳ�ʼ���������г�ʼ����
����ӻ����ͳ�ʼ����Ϣ��ȷ�����ӡ�
��ʼ��������ֹ��������
*/
void initSerial(string port, uint baud = gengeralBaudRate);
void sendInfo2Slave(float x, float y, float z, float frame);



int main(int argc, char** argv)
{
	float t[4];
	initSerialArg(argc, argv);
	while(true)
	{
		string tmp;
		/*for (int i = 0; i < 4;++i)
		{
			cin >> t[i];
		}
		sendInfo2Slave(t[0], t[1], t[2], t[3]);*/
		cin >> tmp;
		slave.write(tmp + '\n');
		Sleep(10);
		tmp = "";
		slave.readline(tmp);
		/*while(slave.available())
		{
			cout << slave.read();
		}*/
		cout << tmp << endl;
	}
}

void sendInfo2Slave(float x, float y, float z, float frame)
{
	int16_t data[4];
	data[0] = static_cast<int16_t>(x * 10.0 + 0.5);
	data[1] = static_cast<int16_t>(y * 10.0 + 0.5);
	data[2] = static_cast<int16_t>(z * 10.0 + 0.5);
	data[3] = static_cast<int16_t>(frame + 0.5);
	uint8_t frameData[20];
	auto size = makeDataFrame(data, frameData, 4 * sizeof(int16_t));
	slave.write(frameData, size);
	printf("(%.1f, %.1f, %.1f)\r", x, y, z);
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud;
	if (argc == 1)
	{// ������δ�ṩ����������Ļ����
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// �����в���ֻ�ṩ�˴��ڣ�������Ĭ��
		port = string(argv[1]);
		baud = gengeralBaudRate;
	}
	else if (argc >= 3)
	{// �����в����ṩ�˴��ںͲ�����
		port = string(argv[1]);
		sscanf_s(argv[2], "%u", &baud);
	}
	try
	{
		initSerial(port, baud);
	}
	catch (exception& err)
	{
		cerr << "Unhandled Exception: " << err.what() << endl;
		exit(1);
	}
}

void initSerial(string port, uint baud)
{
	cout << port << ' ' << baud << endl;
	// ���ô������Բ�����
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// ��⿪��״̬
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	return;
	// ��ӻ�����������Ϣ
	slave.write(string(testComHost) + "\n");
	cout << testComHost << endl;
	// �����յ�������Ϣ
	Sleep(1000);
	while (!slave.available());
	for (auto i = 0; i < 5; ++i)
	{
		if(!slave.available())
		{
			--i;
			continue;
		}
		string tmp;
		slave.readline(tmp);
		cout << tmp << endl;
		if (tmp.find(testComSlave) != string::npos)
		{
			cout << "���ڳ�ʼ���ɹ���" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;
	return;

serialErr:
	cerr << "�޷��򿪴���:" << slave.getPort();
	system("pause");
	exit(1);
}