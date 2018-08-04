#ifndef TEST_UDP_SYCN_CLIENT_H
#define TEST_UDP_SYCN_CLIENT_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S



// block client --------------------------------------------------------------------------------------------------
class __TestUdpSyncClientRun : public IThreadRun
{
private:
	virtual void run()
	{
		m_is_exit = false;

		initSocketLib();
		m_sapi = new UdpSyncApi();

		__run();

		releaseSocketLib();
		delete m_sapi;
		m_sapi = NULL;
		printf("client:: exit\n");
	}

	virtual void stop()
	{
		m_is_exit = true;
		if (m_sapi == NULL)
			return;
		m_sapi->udp_close(m_sid);
	}

	void __run()
	{
		if (!m_sapi->udp_open(&m_sid))
		{
			printf("client:: fail to m_sapi->openSocket\n");
			return;
		}
		printf("client:: open socket ok\n");

		if (m_is_exit)
			return;

		__run_testSend();
	}

	void __run_empty()
	{
		while (!m_is_exit)
		{
			Sleep(100 * 1);
		}
	}

	void __run_testSend()
	{
		while (!m_is_exit)
		{
			Sleep(100);
			Sleep(2 * 1000);
			printf("send hello\n");
			if (!__sendPackToSvr("hello, i am client, how are you?"))
				return;
		}
	}

	bool __sendPackToSvr(const std::string& str)
	{
		if (!m_sapi->udp_sendTo(m_sid, (const byte_t*)str.c_str(), str.size() + 1, SocketUtil::strToIp("127.0.0.1"), 12306))
		{
			printf("client:: fail to send\n");
			return false;
		}
		return true;
	}

	bool __sendPackToSvr(const byte_t* data, size_t data_len)
	{
		if (!m_sapi->udp_sendTo(m_sid, data, data_len, SocketUtil::strToIp("0.0.0.0"), 12306))
		{
			printf("client:: fail to send\n");
			return false;
		}
		return true;
	}





	bool m_is_exit;
	UdpSyncApi* m_sapi;
	socket_id_t m_sid;
};


void __testUdpSyncClient()
{
	printf("\n__TestUdpSyncClient ---------------------------------------------------------\n");
	__initLog(ELogLevel_info);
	__TestUdpSyncClientRun* r = new __TestUdpSyncClientRun();
	Thread* t = new Thread(r);
	t->start();
	__pauseConsole();
	t->stopAndJoin();
	delete t;
}











#endif // !TESTSOCKETCLIENT_H

