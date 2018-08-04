#ifndef TEST_UDP_ASYNC_CLIENT_H
#define TEST_UDP_ASYNC_CLIENT_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S





// callback client --------------------------------------------------------------------------------------------------
class __TestUdpAsyncClientLogic : public IConsoleAppLogic
{
private:
	virtual void onAppStartMsg(IConsoleAppApi * api) override
	{
		printf("client:: onAppStartMsg\n");

		m_console_api = api;
		initSocketLib();
		m_sapi = new SocketApi();
		if (!m_sapi->init(&(m_console_api->getMessageLooper())))
		{
			printf("client:: fail to m_sapi->init\n");
			m_console_api->exit();
			return;
		}

		m_sid = 0;
		IUdpAsyncApi::UdpSocketCreateParam param;
		param.m_notify_looper = &m_console_api->getMessageLooper();
		param.m_notify_target = this;
		if (!m_sapi->audp_createSocket(&m_sid, param))
		{
			printf("client:: fail to m_sapi->createClientSocket\n");
			m_console_api->exit();
			return;
		}

		m_timer_id = m_console_api->getMessageLooper().createTimer(nullptr);
		
		m_console_api->getMessageLooper().startTimer(m_timer_id, 1, 1000);
	}

	virtual void onAppStopMsg() override
	{
		printf("client:: exiting...\n");
		delete m_sapi;
		releaseSocketLib();
		printf("client:: exited...\n");
	}

	virtual void onTextMsg(const std::string & textMsg) override
	{
		//if (!m_sapi->sendDataFromClientSocketToSvr(m_sid, (const byte_t*)textMsg.c_str(), textMsg.size() + 1))
		//{
		//	printf("client:: fail to sendDataFromClientSocketToSvr\n");
		//}
	}

	virtual void onMessage(Message * msg, bool* is_handled) override
	{
		if (msg->m_target != this)
			return;

		if (msg->m_sender == m_sapi)
		{
			switch (msg->m_msg_type)
			{
			case IUdpAsyncApi::EMsgType_udpSocketClosed:
				printf("client:: socket closed\n");
				break;

			case IUdpAsyncApi::EMsgType_udpSocketRecvData:
				__onMsg_recvData(msg);
				break;

			case IUdpAsyncApi::EMsgType_udpSocketSendDataEnd:
				__onMsg_sendDataEnd(msg);
				break;
			default:
				break;
			}
		}
	}

	virtual void onMessageTimerTick(uint64_t timer_id, void* user_data) override 
	{
		if (timer_id != m_timer_id)
			return;

		__sendPackToSvr();
	}

	void __onMsg_recvData(Message* msg)
	{
		ITcpAsyncClientApi::Msg_TcpClientSocketRecvData* m = (ITcpAsyncClientApi::Msg_TcpClientSocketRecvData*)msg;
		if (m->m_recv_data.getLen() > 10)
		{
			const char* sz = (const char*)m->m_recv_data.getData();
			if (sz[0] == 't' && sz[1] == 'e' && strncmp(sz, "test_case=", strlen("test_case=")) == 0)
			{
				std::string recv_str;
				recv_str.append((const char*)m->m_recv_data.getData());
				printf("client:: recv=%s\n", recv_str.c_str());

				std::string test_case_str = StringUtil::fetchMiddle(recv_str, "test_case=", ",");
				if (test_case_str == "download_end")
				{

				}
			}
		}
	}

	void __onMsg_sendDataEnd(Message* msg)
	{
		printf("client:: send end\n");
	}

	bool __sendPackToSvr(const std::string& str)
	{
		if (!m_sapi->audp_sendTo(m_sid, m_connection_id, (const byte_t*)str.c_str(), str.size() + 1, SocketUtil::strToIp("127.0.0.1"), 12306))
		{
			printf("fail to sendDataFromClientSocketToSvr\n");
			return false;
		}
		return true;
	}

	bool __sendPackToSvr(const byte_t* data, size_t data_len)
	{
		bool is_ok = m_sapi->audp_sendTo(m_sid, m_connection_id, data, data_len, SocketUtil::strToIp("127.0.0.1"), 12306);
		if (!is_ok)
		{
			printf("fail to __sendPackToSvr\n");
			return false;
		}
		return is_ok;
	}

	bool __sendPackToSvr()
	{
		std::string str = std::string() + "hello, i am client, how are you? " + StringUtil::toString(++m_send_count);
		return __sendPackToSvr(str);
	}



	IConsoleAppApi* m_console_api;
	SocketApi* m_sapi;
	socket_id_t m_sid;
	uint64_t m_connection_id;
	uint64_t m_timer_id = 0;
	uint64_t m_send_count = 0;
};







void __testUdpAsyncClient()
{
	printf("\n__testTcpAsyncClient ---------------------------------------------------------\n");
	__initLog(ELogLevel_info);
	ConsoleApp* app = new ConsoleApp();
	__TestUdpAsyncClientLogic* logic = new __TestUdpAsyncClientLogic();
	app->run(logic);
	delete logic;
	delete app;
}




#endif // !TEST_TCP_ASYNC_CLIENT_H

