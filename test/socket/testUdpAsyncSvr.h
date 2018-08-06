#ifndef TEST_UDP_ASYNC_SERVER_H
#define TEST_UDP_ASYNC_SERVER_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S




class __TestUdpAsyncSvrLogic : public IConsoleAppLogic
{
private:
	virtual void onAppStartMsg(IConsoleAppApi * api)
	{
		initSocketLib();

		m_console_api = api;

		m_sapi = new SocketApi();
		if (!m_sapi->init(&(m_console_api->getMessageLooper())))
		{
			printf("svr:: fail to init\n");
			m_console_api->exit();
			return;
		}

		m_svr_sid = INVALID_SOCKET;
		IUdpAsyncApi::UdpSocketCreateParam param;
		param.m_notify_looper = &m_console_api->getMessageLooper();
		param.m_notify_target = this;
		param.m_is_bind = true;
		param.m_bind_ip = SocketUtil::strToIp("127.0.0.1");
		param.m_bind_port = 12306;
		if (!m_sapi->audp_createSocket(&m_svr_sid, param))
		{
			printf("svr::  fail to audp_createSocket\n");
			m_console_api->exit();
			return;
		}

		//if (!m_sapi->audp_bind(m_svr_sid, SocketUtil::strToIp("127.0.0.1"), 12306))
		//{
		//	printf("svr:: fail to audp_bind\n");
		//	m_console_api->exit();
		//	return;
		//}

		printf("svr:: start, port=12306\n");
	}

	virtual void onAppStopMsg()
	{
		printf("svr:: exiting...\n");
		delete m_sapi;
		releaseSocketLib();
		delete_and_erase_collection_elements(&m_tran_ctxs);
		printf("svr:: exit\n");
	}

	virtual void onTextMsg(const std::string& textMsg)
	{
		static int64_t s_send_id_seed = 0;
		for (size_t i = 0; i < m_tran_ctxs.size(); ++i)
		{
			__TranCtx* ctx = m_tran_ctxs[i];
			if (!m_sapi->audp_sendTo(m_svr_sid, ++s_send_id_seed,(const byte_t*)textMsg.c_str(), textMsg.size() + 1, ctx->m_from_ip, ctx->m_from_port))
			{
				printf("svr:: fail to send\n");
			}
		}
	}

	virtual void onMessage(Message * msg, bool* is_handled)
	{
		if (msg->m_target != this)
			return;

		if (msg->m_sender == m_sapi)
		{
			switch (msg->m_msg_type)
			{
			case IUdpAsyncApi::EMsgType_udpSocketClosed:
				__onMsg_udpSocketClosed(msg);
				break;

			case IUdpAsyncApi::EMsgType_udpSocketRecvData:
				__onMsg_recvData(msg);
				break;

			case IUdpAsyncApi::EMsgType_udpSocketSendDataEnd:
				//__onMsg_sendDataEnd(msg);
				break;

			default:
				break;
			}
		}
	}

	void __onMsg_udpSocketClosed(Message* msg)
	{
		IUdpAsyncApi::Msg_UdpSocketClosed* msg_closed = (IUdpAsyncApi::Msg_UdpSocketClosed*)msg;
		printf("svr:: socket closed, sid=%" PRIu64 "\n", msg_closed->m_sid);
	}

	void __onMsg_recvData(Message* msg)
	{
		IUdpAsyncApi::Msg_UdpSocketRecvData* msg_recv_data = (IUdpAsyncApi::Msg_UdpSocketRecvData*)msg;
		__TranCtx* tran_ctx = __getTranCtxByFromIpAndPort(msg_recv_data->m_from_ip, msg_recv_data->m_from_port);
		if (tran_ctx == nullptr)
		{
			tran_ctx = new __TranCtx();
			tran_ctx->m_from_ip = msg_recv_data->m_from_ip;
			tran_ctx->m_from_port = msg_recv_data->m_from_port;
			tran_ctx->m_start_time_ms = TimeUtil::getMsTime();
			m_tran_ctxs.push_back(tran_ctx);
		}

		tran_ctx->m_recv_total_len += msg_recv_data->m_recv_data.getLen();

		const char* sz = (const char*)msg_recv_data->m_recv_data.getData();
		printf("svr:: from_ip=%s, from_port=%d, recv=%s\n", SocketUtil::ipToStr(msg_recv_data->m_from_ip).c_str(), msg_recv_data->m_from_port, msg_recv_data->m_recv_data.getData());
	}

	class __TranCtx
	{
	public:
		__TranCtx() { m_recv_total_len = 0; m_start_time_ms = 0; m_already_print_recv_block_count = 0; }
		uint64_t m_recv_total_len;
		uint64_t m_start_time_ms;
		size_t m_already_print_recv_block_count;
		Ip m_from_ip;
		uint32_t m_from_port;
	};

	__TranCtx* __getTranCtxByFromIpAndPort(Ip from_ip, uint32_t from_port)
	{
		for (size_t i = 0; i < m_tran_ctxs.size(); ++i)
		{
			if (m_tran_ctxs[i]->m_from_ip == from_ip && m_tran_ctxs[i]->m_from_port == from_port)
				return m_tran_ctxs[i];
		}
		return nullptr;
	}




	IConsoleAppApi* m_console_api;
	SocketApi* m_sapi;
	socket_id_t m_svr_sid;
	std::vector<__TranCtx*> m_tran_ctxs;
};




void __testUdpAsyncSvr()
{
	printf("\n__testUdpAsyncSvr ---------------------------------------------------------\n");
	__initLog(ELogLevel_debug);
	ConsoleApp* app = new ConsoleApp();
	__TestUdpAsyncSvrLogic* logic = new __TestUdpAsyncSvrLogic();
	app->run(logic);
	delete logic;
	delete app;
}



#endif

