///*
//对电脑进行截屏，把截屏数据保存到剪切板
//*/
//#include <Windows.h>
//#include <stdio.h>
//#include <iostream>
//#include <winsock2.h>
//#include <iostream>
//#include <stdlib.h>
//
//void main()
//{
//
//	//获取桌面大小
//	HDC hdcScreen = ::GetDC(NULL);
//	//HDC hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
//	int nBitPerPixel = GetDeviceCaps(hdcScreen, BITSPIXEL);
//	int nWidth = GetDeviceCaps(hdcScreen, HORZRES);
//	int nHeight = GetDeviceCaps(hdcScreen, VERTRES);
//
//	//建立一个屏幕设备环境句柄
//	HDC hMemDC;
//	HBITMAP hBitmap, hOldBitmap;
//	hMemDC = CreateCompatibleDC(hdcScreen);
//	hBitmap = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
//	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
//
//	//把屏幕设备描述表拷贝到内存设备描述表中
//	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);
//
//	//打开剪贴板，并将位图拷到剪贴板上
//	OpenClipboard(NULL);
//	EmptyClipboard();
//	SetClipboardData(CF_BITMAP, hBitmap);
//	CloseClipboard();
//
//	//释放资源
//	DeleteDC(hdcScreen);
//	DeleteDC(hMemDC);
//	DeleteObject(hBitmap);
//}


/*
获取电脑局域网非回环IP
*/
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdio.h>
//
//#pragma comment(lib, "Ws2_32.lib")
//
//int main() {
//	WSADATA wsaData;
//	char hostname[256];
//	struct addrinfo hints, *res, *ptr;
//	struct sockaddr_in *sockaddr_ipv4;
//	char ipstr[INET_ADDRSTRLEN];
//	bool found = false;
//
//	// 初始化 Winsock 库
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
//		return 1;
//	}
//
//	// 获取主机名
//	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
//		printf("gethostname failed with error: %d\n", WSAGetLastError());
//		WSACleanup();
//		return 1;
//	}
//
//	printf("Host name: %s\n", hostname);
//
//	// 设置 addrinfo 结构
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_INET; // 只处理 IPv4 地址
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	// 获取地址信息
//	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
//		printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
//		WSACleanup();
//		return 1;
//	}
//
//	// 遍历地址信息，寻找第一个非环回地址
//	for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
//		sockaddr_ipv4 = (struct sockaddr_in *)ptr->ai_addr;
//		inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstr, sizeof(ipstr));
//		if (sockaddr_ipv4->sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
//			printf("First non-loopback IPv4 Address: %s\n", ipstr);
//			found = true;
//			break;
//		}
//	}
//
//	// 检查是否找到有效的 IP 地址
//	if (!found) {
//		printf("No non-loopback IPv4 Address found.\n");
//	}
//
//	// 释放地址信息
//	freeaddrinfo(res);
//
//	// 清理 Winsock 库
//	WSACleanup();
//
//	return 0;
//}



//截屏test
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <Windows.h>
//#include <gdiplus.h>
//#include <thread>
//#include <chrono>
//#include <iostream>
//#include <string>
//#include <sstream>
//
//#pragma comment(lib, "Ws2_32.lib") // 显式连接套接字库
//#pragma comment(lib, "Gdiplus.lib") // 显式连接GDI+库
//
//// 获取图像编码器的 CLSID
//int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
//	UINT num = 0;          // 图像编码器的数量
//	UINT size = 0;         // 所需缓冲区的大小
//
//	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
//
//	Gdiplus::GetImageEncodersSize(&num, &size);
//	if (size == 0) return -1;  // 没有找到编码器
//
//	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
//	if (pImageCodecInfo == NULL) return -1;  // 内存不足
//
//	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
//
//	for (UINT j = 0; j < num; ++j) {
//		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
//			*pClsid = pImageCodecInfo[j].Clsid;
//			free(pImageCodecInfo);
//			return j;  // 成功
//		}
//	}
//
//	free(pImageCodecInfo);
//	return -1;  // 失败
//}
//
//// 截屏并保存为 PNG 文件
//void CaptureScreenAndSave(int count) {
//	// 初始化 GDI+
//	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
//	ULONG_PTR gdiplusToken;
//	Gdiplus::Status gdiplusStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
//	if (gdiplusStatus != Gdiplus::Ok) {
//		std::wcerr << L"Failed to initialize GDI+: " << gdiplusStatus << std::endl;
//		return;
//	}
//
//	// 获取桌面窗口的设备上下文
//	HDC hdcScreen = GetDC(NULL);
//	HDC hdcMemory = CreateCompatibleDC(hdcScreen);
//
//	// 获取屏幕尺寸
//	int nWidth = GetDeviceCaps(hdcScreen, HORZRES);
//	int nHeight = GetDeviceCaps(hdcScreen, VERTRES);
//
//	// 创建兼容位图
//	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
//	SelectObject(hdcMemory, hBitmap);
//
//	// 将屏幕内容复制到内存设备上下文
//	BitBlt(hdcMemory, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);
//
//	// 创建 Gdiplus 位图
//	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(hBitmap, NULL);
//
//	// 获取 PNG 编码器的 CLSID
//	CLSID clsidEncoder;
//	if (GetEncoderClsid(L"image/png", &clsidEncoder) == -1) {
//		std::wcerr << L"Failed to get PNG encoder CLSID" << std::endl;
//		delete bitmap;
//		DeleteObject(hBitmap);
//		DeleteDC(hdcMemory);
//		ReleaseDC(NULL, hdcScreen);
//		Gdiplus::GdiplusShutdown(gdiplusToken);
//		return;
//	}
//
//	// 生成文件名
//	std::wstringstream wss;
//	wss << L"screenshot_" << count << L".png";
//	std::wstring filename = wss.str();
//
//	// 保存位图为 PNG 文件
//	Gdiplus::Status status = bitmap->Save(filename.c_str(), &clsidEncoder, NULL);
//	if (status != Gdiplus::Ok) {
//		std::wcerr << L"Failed to save bitmap: " << status << std::endl;
//	}
//	else {
//		std::wcout << L"Screenshot saved as " << filename << std::endl;
//	}
//
//	// 释放资源
//	delete bitmap;
//	DeleteObject(hBitmap);
//	DeleteDC(hdcMemory);
//	ReleaseDC(NULL, hdcScreen);
//	Gdiplus::GdiplusShutdown(gdiplusToken);
//}
//
//int main() {
//	int count = 0;
//	while (true) {
//		// 截屏并保存
//		CaptureScreenAndSave(count++);
//
//		// 每五分钟截屏一次
//		std::this_thread::sleep_for(std::chrono::minutes(5));
//	}
//	return 0;
//}




//真正的项目
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h> // 包含新的地址转换函数头文件
#include <stdio.h>
#include <windows.h>
#include "thread_pool.h"
#include "handleClient.h"

#pragma comment(lib, "WS2_32.lib") // 套接字库 显式连接
int main() {
	WSADATA data;
	WORD w = MAKEWORD(2, 2); // 使用更高版本的 Winsock
	char sztext[] = "beidou";

	// 初始化 Winsock 库
	if (::WSAStartup(w, &data) != 0) {
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		printf("Socket creation failed with error: %d\n", WSAGetLastError());
		::WSACleanup();
		return 1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(51107);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (::bind(listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("Bind failed with error: %d\n", WSAGetLastError());
		::closesocket(listenSocket);
		::WSACleanup();
		return 1;
	}

	if (::listen(listenSocket, 4) == SOCKET_ERROR) {
		printf("Listen failed with error: %d\n", WSAGetLastError());
		::closesocket(listenSocket);
		::WSACleanup();
		return 1;
	}

	printf("服务器已经启动\r\n");

	ThreadPool threadPool(4); // 创建线程池，假设最大线程数为4

	while (true) {
		SOCKET clientSocket = ::accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			int err = WSAGetLastError();
			if (err == WSAEINTR) {
				printf("Accept interrupted, continuing...\n");
				continue;
			}
			if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS) {
				printf("Non-blocking accept or operation in progress, continuing...\n");
				continue;
			}
			printf("Accept failed with error: %d\n", err);
			return 1;
		}

		// 使用线程池处理客户端连接
		threadPool.enqueue([clientSocket]() {
			try {
				Server::handleClient(clientSocket);
			}
			catch (const std::exception& e) {
				printf("Exception in client handler: %s\n", e.what());
			}
			catch (...) {
				printf("Unknown exception in client handler\n");
			}
			::closesocket(clientSocket);
		});
	}

	::closesocket(listenSocket); // 关闭监听套接字句柄
	::WSACleanup(); // 释放套接字库

	return 0;
}