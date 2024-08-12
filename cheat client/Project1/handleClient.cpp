#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "handleClient.h"

namespace Server {
	// 初始化 GDI+ 需要的全局变量
	ULONG_PTR gdiplusToken;
	/*
	函数名:GetMyIPv4
	作者:  陈绩
	作用：获取本电脑IP地址的函数，只是给用户查看IP提供方便
	*/
	void GetMyIPv4() {
		WSADATA wsaData;
		char hostname[256];
		struct addrinfo hints, *res, *ptr;
		struct sockaddr_in *sockaddr_ipv4;
		char ipstr[INET_ADDRSTRLEN];
		bool found = false;

		// 初始化 Winsock 库
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			printf("WSAStartup failed with error: %d\n", WSAGetLastError());
			return ;
		}

		// 获取主机名
		if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
			printf("gethostname failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			return ;
		}

		printf("Host name: %s\n", hostname);

		// 设置 addrinfo 结构
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET; // 只处理 IPv4 地址
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// 获取地址信息
		if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
			printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			return ;
		}

		// 遍历地址信息，寻找第一个非环回地址
		for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
			sockaddr_ipv4 = (struct sockaddr_in *)ptr->ai_addr;
			inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstr, sizeof(ipstr));
			if (sockaddr_ipv4->sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
				printf("First non-loopback IPv4 Address: %s\n", ipstr);
				found = true;
				break;
			}
		}

		// 检查是否找到有效的 IP 地址
		if (!found) {
			printf("No non-loopback IPv4 Address found.\n");
		}

		// 释放地址信息
		freeaddrinfo(res);

		// 清理 Winsock 库
		WSACleanup();
		return;
	}
	void GetResult(SOCKET clientSocket)
	{
		unsigned int resultSize;
		if (::recv(clientSocket, reinterpret_cast<char*>(&resultSize), sizeof(resultSize), 0) <= 0) {
			std::cerr << "Failed to receive result size" << std::endl;
			return;
		}

		// 创建一个足够大的缓冲区来接收结果
		char* buffer = new char[resultSize + 1]; // +1 for null terminator
		if (::recv(clientSocket, buffer, resultSize, 0) <= 0) {
			std::cerr << "Failed to receive result" << std::endl;
			delete[] buffer; // 确保释放已分配的内存
			return;
		}
		buffer[resultSize] = '\0'; // 确保字符串以null结尾

		// 将接收到的结果转换为std::string
		std::string result(buffer);
		delete[] buffer; // 释放缓冲区内存

		// 打开剪贴板
		if (!OpenClipboard(NULL)) {
			std::cerr << "Failed to open clipboard" << std::endl;
			return;
		}

		// 清空剪贴板
		if (!EmptyClipboard()) {
			std::cerr << "Failed to empty clipboard" << std::endl;
			CloseClipboard();
			return;
		}

		// 分配全局内存对象并将结果复制到剪贴板
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, result.size() + 1);
		if (hClipboardData == NULL) {
			std::cerr << "Failed to allocate global memory" << std::endl;
			CloseClipboard();
			return;
		}

		// 将字符串复制到全局内存
		char* pClipboardData = static_cast<char*>(GlobalLock(hClipboardData));
		memcpy(pClipboardData, result.c_str(), result.size() + 1);
		GlobalUnlock(hClipboardData);

		// 设置剪贴板数据
		if (SetClipboardData(CF_TEXT, hClipboardData) == NULL) {
			std::cerr << "Failed to set clipboard data" << std::endl;
			GlobalFree(hClipboardData);
			CloseClipboard();
			return;
		}

		// 关闭剪贴板
		CloseClipboard();

		std::cout << "Result has been copied to the clipboard" << std::endl;
	}
	// 从剪贴板获取图像
	HBITMAP GetClipboardImage() {
		// 获取桌面大小
		HDC hdcScreen = ::GetDC(NULL);
		int nWidth = GetDeviceCaps(hdcScreen, HORZRES);
		int nHeight = GetDeviceCaps(hdcScreen, VERTRES);

		// 建立一个屏幕设备环境句柄
		HDC hMemDC = CreateCompatibleDC(hdcScreen);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		// 把屏幕设备描述表拷贝到内存设备描述表中
		BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);

		// 释放资源
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hdcScreen);
		DeleteDC(hMemDC);

		return hBitmap;
	}

	// 获取图像编码器的 CLSID
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		UINT num = 0;          // 图像编码器的数量
		UINT size = 0;         // 所需缓冲区的大小

		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0) return -1;  // 没有找到编码器

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL) return -1;  // 内存不足

		Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // 成功
			}
		}

		free(pImageCodecInfo);
		return -1;  // 失败
	}

	// 将 HBITMAP 转换为 PNG 格式并发送给客户端
	void SendImageData(SOCKET clientSocket, HBITMAP hBitmap) {
		if (hBitmap == NULL) {
			printf("No image data available\n");
			return;
		}

		// 初始化 GDI+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
			printf("GdiplusStartup failed\n");
			return;
		}

		{
			// 将 HBITMAP 转换为 Gdiplus::Bitmap
			Gdiplus::Bitmap bmp(hBitmap, NULL);

			// 创建 IStream 以保存 PNG 数据
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) != S_OK) {
				printf("CreateStreamOnHGlobal failed\n");
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 设置图像编码器参数
			CLSID clsidEncoder;
			if (GetEncoderClsid(L"image/png", &clsidEncoder) == -1) {
				printf("Failed to get encoder CLSID\n");
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 将位图保存为 PNG 格式
			if (bmp.Save(pStream, &clsidEncoder, NULL) != Gdiplus::Ok) {
				printf("Failed to save bitmap to stream\n");
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 获取 PNG 数据的大小
			STATSTG statstg;
			if (pStream->Stat(&statstg, STATFLAG_DEFAULT) != S_OK) {
				printf("Failed to get stream stat\n");
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}
			ULONG dataSize = statstg.cbSize.LowPart;

			// 分配内存以存储 PNG 数据
			BYTE* pData = new BYTE[dataSize];
			LARGE_INTEGER liZero = {};
			if (pStream->Seek(liZero, STREAM_SEEK_SET, NULL) != S_OK) {
				printf("Failed to seek to beginning of stream\n");
				delete[] pData;
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}
			ULONG bytesRead;
			if (pStream->Read(pData, dataSize, &bytesRead) != S_OK || bytesRead != dataSize) {
				printf("Failed to read from stream\n");
				delete[] pData;
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 发送 PNG 数据的大小
			if (send(clientSocket, (char*)&dataSize, sizeof(dataSize), 0) == SOCKET_ERROR) {
				printf("Failed to send data size\n");
				delete[] pData;
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 发送 PNG 数据
			if (send(clientSocket, (char*)pData, dataSize, 0) == SOCKET_ERROR) {
				printf("Failed to send image data\n");
				delete[] pData;
				pStream->Release();
				Gdiplus::GdiplusShutdown(gdiplusToken);
				return;
			}

			// 清理
			delete[] pData;
			pStream->Release();
		}

		// 关闭 GDI+
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	void handleClient(SOCKET clientSocket) {
		sockaddr_in addr2;
		int n = sizeof(addr2);
		getpeername(clientSocket, (sockaddr*)&addr2, &n);
		printf("%s 已经连接上\r\n", inet_ntoa(addr2.sin_addr));
		while (true) {
			// 接收命令数字
			int order;
			if (::recv(clientSocket, reinterpret_cast<char*>(&order), sizeof(order), 0) <= 0) {
				printf("Failed to receive data size\n");
				::closesocket(clientSocket);
				return;
			}
			std::cout << "order=" << order << std::endl;
			// 获取剪贴板图像并发送
			switch (order){
			case Client::picture: {
				HBITMAP hBitmap = GetClipboardImage();
				SendImageData(clientSocket, hBitmap);
				break;
			}
			case Client::result:{
				GetResult(clientSocket);
				break;
			}
			default: {
				break;
			}
			}
		}
		::closesocket(clientSocket); // 关闭客户端套接字句柄 
	}
}