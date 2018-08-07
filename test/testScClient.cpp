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
#include "socket/testUdpSyncClient.h"
#include "socket/testUdpAsyncClient.h"
using namespace std;
USING_NAMESPACE_S



int main(int argc, char** argv)
{
	printf("test start **************************************************************\n");


	//__testDnsApi();
	//__testDnsResolver();
	//__testTcpConnector();

	//__testTcpAsyncClient();
	//__testTcpRawClient();
	//__testTcpSyncClient();
	__testTcpAsyncClient();
	//__testIocpClient();

	//__testUdpSyncClient();
	//__testUdpAsyncClient();



	printf("test end **************************************************************\n");
	return 0;
}



