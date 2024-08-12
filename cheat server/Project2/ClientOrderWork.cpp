#include "ClientOrderWork.h"
namespace Client {
	bool GetProblemPNG(SOCKET s) {
		// 服务器端接受命令成功,接收图片大小
		unsigned int PNGSize;
		if (::recv(s, reinterpret_cast<char*>(&PNGSize), sizeof(PNGSize), 0) <= 0) {
			printf("Failed to receive PNG size\n");
			return false;
		}
		// 分配内存以接收图片数据
		char *PNGData = new char[PNGSize];
		if (!PNGData) {
			printf("Failed to malloc memory for PNG\n");
			return false;
		}
		unsigned int charReceivedSize = 0;
		while (charReceivedSize < PNGSize) {
			int result = ::recv(s, PNGData + charReceivedSize, PNGSize - charReceivedSize, 0);
			if (result <= 0)
			{
				printf("Failed to receive PNGData\n");
				return false;
			}
			charReceivedSize += result;
		}

		// 生成文件名
		//std::string currentTimeStr = getCurrentTimeStr();
		//std::string fileName = "problem_" + currentTimeStr + ".png";


		std::string currentProblemNumStr = getProblemNumStr();
		std::string fileName = "problem_" + currentProblemNumStr + ".png";

		// 保存图片数据到文件
		std::ofstream outFile(fileName, std::ios::binary);
		if (!outFile) {
			std::cerr << "Failed to open file for writing" << std::endl;
			delete[] PNGData;
			return false;
		}
		outFile.write(PNGData, PNGSize);
		outFile.close();

		std::cout << "图片已接收并保存为 " << fileName << std::endl;

		// 释放内存
		delete[] PNGData;
		return true;
	}
	bool SendResultText(SOCKET s){
		std::cin.get(); // 读取一个字符（回车）

		// 从标准输入获取结果字符串
		
		std::cout << "确保你的剪切版有你的答案\n ";
		// 从剪贴板获取内容
		std::string result = GetClipboardText();
		std::cout << "这是你给服务器端发送的答案:" << result <<std::endl;
		// 获取字符串长度
		uint32_t resultSize = static_cast<uint32_t>(result.size());

		// 发送字符串长度
		if (send(s, reinterpret_cast<char*>(&resultSize), sizeof(resultSize), 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send result size. Error: " << WSAGetLastError() << std::endl;
			return false;
		}
		// 发送字符串内容
		if (send(s, result.c_str(), resultSize, 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send result data. Error: " << WSAGetLastError() << std::endl;
			return false;
		}

		return true;
	}
	std::string getCurrentTimeStr() {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm local_tm;
		localtime_s(&local_tm, &in_time_t);

		std::stringstream ss;
		ss << std::put_time(&local_tm, "%Y%m%d%H%M%S");
		return ss.str();
	}
	std::string getProblemNumStr() {
		static int problemNum = 0;
		problemNum += 1; // 递增问题编号
		std::stringstream ss;
		ss << problemNum;
		return ss.str(); // 返回字符串表示的编号
	}
	std::string GetClipboardText() {
		if (!OpenClipboard(NULL)) {
			return "";
		}

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == NULL) {
			CloseClipboard();
			return "";
		}

		char* pszText = static_cast<char*>(GlobalLock(hData));
		if (pszText == NULL) {
			CloseClipboard();
			return "";
		}

		std::string text(pszText);

		GlobalUnlock(hData);
		CloseClipboard();

		return text;
	}
}
