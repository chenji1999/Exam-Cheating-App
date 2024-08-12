#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h> // 包含新的地址转换函数头文件
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <gdiplus.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>
#include <ctime>
#pragma comment(lib, "Gdiplus.lib")
namespace Client {
	bool GetProblemPNG(SOCKET s);
	bool SendResultText(SOCKET s);
	std::string getCurrentTimeStr();
	std::string getProblemNumStr();
	std::string GetClipboardText();
}

