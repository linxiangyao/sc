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
#define __TestSocketBlockSvrSpeedRun_recvBufLen (64 * 1024)
	virtual void run()
	{
		m_is_exit = false;
		m_buf = new byte_t[__TestSocketBlockSvrSpeedRun_recvBufLen];
		initSocketLib();
		m_sapi = new TcpSyncApi();

		__run();

		delete m_sapi;
		releaseSocketLib();
		delete[] m_buf;
	}

	virtual void stop()
	{
		m_is_exit = true;
		m_sapi->tcp_close(m_listen_sid);
		m_sapi->tcp_close(m_tran_sid);
	}

	void __run()
	{
		if (!m_sapi->tcp_open(&m_listen_sid))
		{
			printf("svr:: fail to openSocket\n");
			return;
		}
		printf("svr:: openSocket ok\n");

		if (!m_sapi->tcp_bindAndListen(m_listen_sid, "0.0.0.0", 12306))
		{
			printf("svr:: fail to bindAndListen\n");
			return;
		}
		printf("svr:: bindAndListen ok, port=12306\n");

		while (true)
		{
			if (m_is_exit)
				return;

			printf("svr:: start accept\n");
			if (!m_sapi->tcp_accept(m_listen_sid, &m_tran_sid))
			{
				printf("svr:: fail to accept\n");
				return;
			}
			printf("svr:: accept one client\n");

			__runTran();
		}
	}

	void __runTran()
	{
		uint64_t recv_total_len = 0;
		uint64_t start_time_ms = TimeUtil::getMsTime();
		m_already_print_recv_block_count = 0;
		while (true)
		{
			Sleep(1* 1000);

			size_t recv_len = 0;
			if (!m_sapi->tcp_recv(m_tran_sid, m_buf, __TestSocketBlockSvrSpeedRun_recvBufLen, &recv_len))
			{
				printf("svr:: fail to recv\n");
				break;
			}
			recv_total_len += recv_len;
			printf("*");

			__onRecvPack(recv_total_len);

			if (recv_len > 10)
			{
				const char* sz = (const char*)m_buf;
				if (sz[0] == 't' && sz[1] == 'e' && strncmp(sz, "test_case=", strlen("test_case=")) == 0)
				{
					printf("\nsvr:: recv=%s\n", sz);
				}
			}
		}

		uint64_t total_time_span_ms = TimeUtil::getMsTime() - start_time_ms;
		uint64_t byte_per_second = recv_total_len * 1000 / total_time_span_ms;
		printf("\nsvr:: recv_total_len=%s, recv_total_ms=%" PRIu64 ", byte_per_second=%s\n"
			, StringUtil::byteCountToDisplayString(recv_total_len).c_str(), total_time_span_ms, StringUtil::byteCountToDisplayString(byte_per_second).c_str());
	}

	void __onRecvPack(uint64_t recv_total_len)
	{
		const size_t block_size = 1 * 1024 * 1024;
		size_t recv_block_count = (size_t)recv_total_len / block_size;
		while (recv_block_count > m_already_print_recv_block_count)
		{
			++m_already_print_recv_block_count;
			printf(".");
			fflush(stdout);
		}
	}



	bool m_is_exit;
	TcpSyncApi* m_sapi;
	socket_id_t m_listen_sid;
	socket_id_t m_tran_sid;
	byte_t* m_buf;
	size_t m_already_print_recv_block_count = 0;
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





#endif // !TESTSOCKETSVR_H

