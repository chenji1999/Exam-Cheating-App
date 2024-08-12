#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h> //包含头文件 
#include <stdio.h>
#include <windows.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>
#include <ctime>
#include "ClientOrderEnum.h"
#include "ClientOrderWork.h"
#pragma comment(lib,"WS2_32.lib") //显式连接套接字库 
std::string getCurrentTimeStr() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm;
	localtime_s(&local_tm, &in_time_t);

	std::stringstream ss;
	ss << std::put_time(&local_tm, "%Y%m%d%H%M%S");
	return ss.str();
}
int main() {
	WSADATA data; // 定义WSADATA结构体对象
	WORD w = MAKEWORD(2, 2); // 定义版本号码
	if (::WSAStartup(w, &data) != 0) {
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	SOCKET s; // 定义连接套接字句柄
	s = ::socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
	if (s == INVALID_SOCKET) {
		printf("Socket creation failed with error: %d\n", WSAGetLastError());
		::WSACleanup();
		return 1;
	}
	std::string serverIp;
	std::cout << "输入你想作弊的电脑的IP: ";
	std::cin >> serverIp;

	sockaddr_in addr; // 定义套接字地址结构
	addr.sin_family = AF_INET; // 初始化地址结构
	addr.sin_port = htons(51107);
	addr.sin_addr.S_un.S_addr = inet_addr(serverIp.c_str());
	printf("客户端已经启动\r\n"); // 输出提示信息

	if (::connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("Connect failed with error: %d\n", WSAGetLastError());
		::closesocket(s);
		::WSACleanup();
		return 1;
	}
	std::cout << "1:表示考试题目截屏" << std::endl;
	std::cout << "2:表示考试答案输出文本\n" << std::endl;
	while (true){
		int order;
		std::cout << "请输入命令数字：" << std::endl;
		std::cin >> order;
		if (send(s, reinterpret_cast<char*>(&order), sizeof(order), 0) == SOCKET_ERROR) {
			printf("Failed to send order\n");
			return 0;
		}
		bool flag = false;
		switch (order)
		{
		case Client::picture: {
			flag = Client::GetProblemPNG(s);
			if (!flag) {
				::closesocket(s);
				::WSACleanup();
				return 0;
			}
			break;
		}
		case Client::result: {
			flag = Client::SendResultText(s);
			if (!flag) {
				::closesocket(s);
				::WSACleanup();
				return 0;
			}
			break;
		}
		default: {
			//::closesocket(s);
			//::WSACleanup();
			break;
		}
		}
	 //   // 服务器端接受命令成功,接收图片大小
		//unsigned int PNGSize;
		//if (::recv(s, reinterpret_cast<char*>(&PNGSize), sizeof(PNGSize), 0) <= 0) {
		//	printf("Failed to receive PNG size\n");
		//	::closesocket(s);
		//	::WSACleanup();
		//	return 0;
		//}
		//// 分配内存以接收图片数据
		//char *PNGData = new char[PNGSize];
		//if (!PNGData){
		//	printf("Failed to malloc memory for PNG\n");
		//	::closesocket(s);
		//	::WSACleanup();
		//	return 0;
		//}
		//unsigned int charReceivedSize=0;
		//while (charReceivedSize < PNGSize) {
		//	int result = ::recv(s, PNGData+charReceivedSize, PNGSize-charReceivedSize, 0);
		//	if (result <= 0)
		//	{
		//		printf("Failed to receive PNGData\n");
		//		::closesocket(s);
		//		::WSACleanup();
		//		return 0;
		//	}
		//	charReceivedSize += result;
		//}
		//
		//// 生成文件名
		//std::string currentTimeStr = getCurrentTimeStr();
		//std::string fileName = "problem_" + currentTimeStr + ".png";

		//// 保存图片数据到文件
		//std::ofstream outFile(fileName, std::ios::binary);
		//if (!outFile) {
		//	std::cerr << "Failed to open file for writing" << std::endl;
		//	delete[] PNGData;
		//	closesocket(s);
		//	WSACleanup();
		//	return 1;
		//}
		//outFile.write(PNGData, PNGSize);
		//outFile.close();

		//std::cout << "图片已接收并保存为 " << fileName << std::endl;

		//// 释放内存
		//delete[] PNGData;
	}
	return 0;
}