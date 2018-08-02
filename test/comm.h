#ifndef TEST_S_H
#define TEST_S_H
#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>




void __pauseConsoleAndGetInput(std::string* strResult)
{
	while (true)
	{
		char c = 0;
		std::cin.read(&c, 1);

		if (strResult != NULL) {
			strResult->append(&c, 1);
		}
		if (c == '\n')
			break;
	}
}

void __pauseConsole()
{
	__pauseConsoleAndGetInput(NULL);
}

//void __print(const char* s);
//void __sleepAndPrint(int ms, const char* sz);
//void __sleepAndPrintAndPauseConsole(int ms, const char* sz);

//#define __print(printMsg, ...) { printf(printMsg, __VA_ARGS__); printf("\n"); }
//#define __sleepAndPrint(sleepMs, printMsg, ...) { Thread::sleep(sleepMs); __print(printMsg, __VA_ARGS__); }
//#define __sleepAndPrintAndPauseConsole(sleepMs, printMsg, ...) { __sleepAndPrint(sleepMs, printMsg, __VA_ARGS__); __pauseConsole(); }
//#define __sleepAndPrintAndPauseConsoleAndGetInput(sleepMs, inputStr, printMessage, ...) { __sleepAndPrint(sleepMs, printMessage, __VA_ARGS__); __pauseConsole(inputStr); }


#define __print(...) { printf(__VA_ARGS__); }
#define __sleepAndPrint(sleepMs, ...) { Thread::sleep(sleepMs); __print(__VA_ARGS__); }
#define __sleepAndPrintAndPauseConsole(sleepMs, ...) { __sleepAndPrint(sleepMs, __VA_ARGS__); __pauseConsole(); }
#define __sleepAndPrintAndPauseConsoleAndGetInput(sleepMs, inputStr, ...) { __sleepAndPrint(sleepMs, __VA_ARGS__); __pauseConsoleAndGetInput(inputStr); }


// no auth -----------------------------------------------------
#define __ECgiCmdType_c2sNotify_notifyStatistic 1001

#define __ECgiCmdType_c2sReq_checkVersion 2001
#define __ECgiCmdType_c2sReq_downloadClient 2002
#define __ECgiCmdType_c2sReq_login 2003

#define __ECgiCmdType_s2cResp_checkVersion 3001
#define __ECgiCmdType_s2cResp_downloadClient 3002
#define __ECgiCmdType_s2cResp_login 3003



// authed --------------------------------------------------
#define __ECgiCmdType_c2sNotify_notifyWriting 5001

#define __ECgiCmdType_c2sReq_heartBeat 6001
#define __ECgiCmdType_c2sReq_init 6002
#define __ECgiCmdType_c2sReq_sync 6003
#define __ECgiCmdType_c2sReq_syncCheck 6004
#define __ECgiCmdType_c2sReq_sendText 6005
#define __ECgiCmdType_c2sReq_downloadImage 6006

#define __ECgiCmdType_s2cResp_heartBeat 7001
#define __ECgiCmdType_s2cResp_init 7002
#define __ECgiCmdType_s2cResp_sync 7003
#define __ECgiCmdType_s2cResp_syncCheck 7004
#define __ECgiCmdType_s2cResp_sendText 7005
#define __ECgiCmdType_s2cResp_downloadImage 7006

#endif



