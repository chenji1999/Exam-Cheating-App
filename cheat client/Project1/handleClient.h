#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h> // 包含新的地址转换函数头文件
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <gdiplus.h>
#include "ClientOrderEnum.h"
#pragma comment(lib, "Gdiplus.lib")
namespace Server {
	void handleClient(SOCKET clientSocket);
	HBITMAP GetClipboardImage();
	void SendImageData(SOCKET clientSocket, HBITMAP hBitmap);
	void GetResult(SOCKET clientSocket);
	void GetMyIPv4();
}