#include "comm.h"
#include "base/testBinary.h"
#include "base/testVariant.h"
#include "base/testFileUtil.h"
#include "base/testStringUtil.h"
#include "base/testMarco.h"
#include "base/testLog.h"
#include "base/testThread.h"
#include "base/testConsole.h"

#include "socket/testSocketUtil.h"
#include "socket/testDnsResolver.h"
#include "socket/testTcpConnector.h"
#include "socket/testTcpAsyncClient.h"
#include "socket/testTcpAsyncSvr.h"
#include "socket/testTcpSyncClient.h"
#include "socket/testTcpSyncSvr.h"
using namespace std;
USING_NAMESPACE_S


int main(int argc, char** argv)
{
	printf("test start **************************************************************\n");



	//__testBinary();
	//__testVarint();
	//__testStringUtil();
	//__testFileUtil();
	//__testMsgLoopThread();
	//__testMsgLoopThreadPerformance();
	//__testLog();
	//__testThread();
	//__testConsole();

	//__testDnsApi();
	//__testDnsResolver();
	//__testTcpConnector();
	//__testTcpSyncClient();
	//__testTcpSyncSvr();
	__initLog(ELogLevel_info);

	initSocketLib();

	for (int i = 0; i < 100; ++i)
	{
		socket_t s = SocketUtil::openSocket(ESocketType_tcp);
		slog_i("s%0=%1", i, s);
		SocketUtil::closeSocket(s);
	}

	printf("test end **************************************************************\n");
	return 0;
}










