#ifndef TEST_UDP_SYNC_SVR_H
#define TEST_UDP_SYNC_SVR_H
#include "../comm.h"
#include "../../src/socket/socketLib.h"
#include "../../src/util/utilLib.h"
using namespace std;
USING_NAMESPACE_S




class __TestUdpSyncSvrRun : public IThreadRun
{
private:
	virtual void run()
	{
		m_is_exit = false;
		initSocketLib();
		m_sapi = new UdpSyncApi();

		__run();

		delete m_sapi;
		releaseSocketLib();
	}

	virtual void stop()
	{
		m_is_exit = true;
		m_sapi->udp_close(m_svr_sid);
	}

	void __run()
	{
		if (!m_sapi->udp_open(&m_svr_sid))
		{
			printf("svr:: fail to openSocket\n");
			return;
		}
		printf("svr:: openSocket ok\n");

		if (!m_sapi->udp_bind(m_svr_sid, SocketUtil::strToIp("0.0.0.0"), 12306))
		{
			printf("svr:: fail to bindAndListen\n");
			return;
		}
		printf("svr:: bind ok, port=12306\n");
		printf("svr:: start recv\n");

		const static int recv_data_len = 1024 * 64;
		byte_t recv_data[recv_data_len];
		while (true)
		{
			if (m_is_exit)
				return;

			Ip from_ip;
			uint32_t from_port;
			size_t real_recv_len = 0;
			if (!m_sapi->udp_recvFrom(m_svr_sid, recv_data, recv_data_len, &from_ip, &from_port, &real_recv_len))
			{
				printf("svr:: fail to udp_recvFrom\n");
				break;
			}

			if (real_recv_len > 10)
			{
				const char* sz = (const char*)recv_data;
				//if (sz[0] == 't' && sz[1] == 'e' && strncmp(sz, "test_case=", strlen("test_case=")) == 0)
				{
					printf("\nsvr:: recv=%s\n", sz);
				}
			}
		}
	}



	bool m_is_exit;
	UdpSyncApi* m_sapi;
	socket_id_t m_svr_sid;
};




void __testUdpSyncSvr()
{
	printf("\n__testUdpSyncSvr ---------------------------------------------------------\n");
	__initLog(ELogLevel_info);
	__TestUdpSyncSvrRun* r = new __TestUdpSyncSvrRun();
	Thread* t = new Thread(r);
	t->start();
	__pauseConsole();
	t->stopAndJoin();
	delete t;
}





#endif

