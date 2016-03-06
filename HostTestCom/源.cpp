#include<string>
#include<iostream>
#include <windows.h>
#include "serial/serial.h"
#include "../Slave/CommonDef.h"
#include "../host/DataFrame.h"
using namespace std;
using namespace serial;

char buf[1024];// 缓冲区
Serial slave;// 从机通信串口

union int16_8
{
	unsigned __int16 i16;
	unsigned __int8 i8[2];
};

/*
通过main函数参数初始化串口。
argv[1]存在则为端口，不存在则由标准输入获取
argv[2]存在则为波特率，不存在则使用默认波特率baudRate
获取端口和波特率后调用initSerial进行进一步初始化
*/
void initSerialArg(int argc, char** argv);
/*
参数port为端口，参数baud为波特率，默认为baurdRate。
使用Serial类提供的初始化方法进行初始化，
并向从机发送初始化信息以确认连接。
初始化错误将终止整个程序。
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
	{// 命令行未提供参数则由屏幕输入
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// 命令行参数只提供了串口，波特率默认
		port = string(argv[1]);
		baud = gengeralBaudRate;
	}
	else if (argc >= 3)
	{// 命令行参数提供了串口和波特率
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
	// 设置串口属性并开启
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// 检测开启状态
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	return;
	// 向从机发送握手信息
	slave.write(string(testComHost) + "\n");
	cout << testComHost << endl;
	// 检查回收的握手信息
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
			cout << "串口初始化成功。" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;
	return;

serialErr:
	cerr << "无法打开串口:" << slave.getPort();
	system("pause");
	exit(1);
}