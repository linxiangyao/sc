#ifndef TEST_TCP_ASYNC_SERVER_H
#define TEST_TCP_ASYNC_SERVER_H
#include "../comm.h"
#include "../../src/socket/socketLib.h"
#include "../../src/util/utilLib.h"
using namespace std;
USING_NAMESPACE_S




class __TestTcpAsyncSvrLogic : public IConsoleAppLogic
{
public:

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

		m_listen_sid = 0;
		ITcpAsyncApi::TcpSvrListenSocketCreateParam param;
		param.m_notify_looper = &m_console_api->getMessageLooper();
		param.m_notify_target = this;
		param.m_svr_ip = "0.0.0.0";
		param.m_svr_port = 12306;
		if (!m_sapi->atcp_createSvrListenSocket(&m_listen_sid, param))
		{
			printf("svr::  fail to createSvrListenSocket\n");
			m_console_api->exit();
			return;
		}

		if (!m_sapi->atcp_startSvrListenSocket(m_listen_sid))
		{
			printf("svr:: fail to startSvrListenSocket\n");
			m_console_api->exit();
			return;
		}

		printf("svr:: start, port=12306\n");
	}

	virtual void onAppStopMsg()
	{
		printf("svr:: exiting...\n");
		delete m_sapi;
		releaseSocketLib();
		printf("svr:: exit\n");
	}

	virtual void onTextMsg(const std::string& textMsg)
	{
		for (size_t i = 0; i < m_tran_ctxs.size(); ++i)
		{
			if (!m_sapi->atcp_sendDataFromSvrTranSocketToClient(m_tran_ctxs[i]->m_tran_sid, (const byte_t*)textMsg.c_str(), textMsg.size() + 1))
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
			case ITcpAsyncApi::EMsgType_tcpSvrListenSocketStarted:
				printf("svr:: started, wait client...\n");
				break;

			case ITcpAsyncApi::EMsgType_tcpSvrListenSocketStopped:
				printf("svr:: server stopped\n");
				break;

			case ITcpAsyncApi::EMsgType_tcpSvrListenSocketAccepted:
				__onMsg_accept(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpSvrTranSocketStopped:
				__onMsg_tranSocketStopped(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpSvrTranSocketRecvData:
				__onMsg_recvData(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpSvrTranSocketSendDataEnd:
				__onMsg_sendDataEnd(msg);
				break;

			default:
				break;
			}
		}
	}

	void __onMsg_accept(Message* msg)
	{
		ITcpAsyncServerApi::Msg_TcpSvrListenSocketAccepted* m = (ITcpAsyncServerApi::Msg_TcpSvrListenSocketAccepted*)msg;
		printf("\nsvr:: accept one client, sid=%" PRIu64 "\n", m->m_svr_trans_sid);

		__TranCtx* tran_ctx = new __TranCtx();
		tran_ctx->m_tran_sid = m->m_svr_trans_sid;
		tran_ctx->m_start_time_ms = TimeUtil::getMsTime();
		std::string str = "hello, i am svr";
		if (!m_sapi->atcp_sendDataFromSvrTranSocketToClient(tran_ctx->m_tran_sid, (const byte_t*)str.c_str(), str.size() + 1))
		{
			printf("fail to atcp_sendDataFromSvrTranSocketToClient\n");
		}
		m_tran_ctxs.push_back(tran_ctx);
	}

	void __onMsg_tranSocketStopped(Message* msg)
	{
		ITcpAsyncServerApi::Msg_TcpSvrTranSocketStopped* msg_stopped = (ITcpAsyncServerApi::Msg_TcpSvrTranSocketStopped*)msg;
		printf("svr:: client disconnected, sid=%" PRIu64 "\n", msg_stopped->m_svr_trans_sid);

		__TranCtx* tran_ctx = __getTranCtxBySid(msg_stopped->m_svr_trans_sid);
		if (tran_ctx == nullptr)
			return;

		uint64_t total_time_span_ms = TimeUtil::getMsTime() - tran_ctx->m_start_time_ms;
		uint64_t byte_per_second = tran_ctx->m_recv_total_len * 1000 / total_time_span_ms;
		printf("svr:: sid=%" PRIu64 ", recv_total_len=%s, recv_total_ms=%" PRIu64 ", byte_per_second=%s\n"
			, msg_stopped->m_svr_trans_sid, StringUtil::byteCountToDisplayString(tran_ctx->m_recv_total_len).c_str(), total_time_span_ms, StringUtil::byteCountToDisplayString(byte_per_second).c_str());

		delete_and_erase_vector_element_by_value(&m_tran_ctxs, tran_ctx);
	}

	void __onMsg_recvData(Message* msg)
	{
		ITcpAsyncServerApi::Msg_TcpSvrTranSocketRecvData* m = (ITcpAsyncServerApi::Msg_TcpSvrTranSocketRecvData*)msg;
		__TranCtx* tran_ctx = __getTranCtxBySid(m->m_svr_trans_sid);
		if (tran_ctx == nullptr)
			return;

		tran_ctx->m_recv_total_len += m->m_data.getLen();

		const size_t block_size = 1 * 1024 * 1024;
		size_t recv_block_count = (size_t)tran_ctx->m_recv_total_len / block_size;
		while (recv_block_count > tran_ctx->m_already_print_recv_block_count)
		{
			++tran_ctx->m_already_print_recv_block_count;
			printf(".");
			fflush(stdout);
		}
		//printf("*");
		//fflush(stdout);

		const char* sz = (const char*)m->m_data.getData();
		if (sz[0] == 't' && sz[1] == 'e' && strncmp(sz, "test_case=", strlen("test_case=")) == 0)
		{
			if (tran_ctx->m_already_print_recv_block_count > 0)
				printf("\n");
			printf("svr:: sid=%" PRIu64 ", recv=%s\n", m->m_svr_trans_sid, m->m_data.getData());
		}
	}

	void __onMsg_sendDataEnd(Message* msg)
	{
		//printf("svr:: send end\n");
	}

	class __TranCtx
	{
	public:
		__TranCtx() { m_tran_sid = 0; m_recv_total_len = 0; m_start_time_ms = 0; m_already_print_recv_block_count = 0; }
		socket_id_t m_tran_sid;
		uint64_t m_recv_total_len;
		uint64_t m_start_time_ms;
		size_t m_already_print_recv_block_count;
	};

	__TranCtx* __getTranCtxBySid(socket_id_t sid)
	{
		for (size_t i = 0; i < m_tran_ctxs.size(); ++i)
		{
			if (m_tran_ctxs[i]->m_tran_sid == sid)
				return m_tran_ctxs[i];
		}
		return nullptr;
	}




	IConsoleAppApi* m_console_api;
	SocketApi* m_sapi;
	socket_id_t m_listen_sid;
	std::vector<__TranCtx*> m_tran_ctxs;
};




void __testTcpAsyncSvr()
{
	printf("\n__testTcpAsyncSvr ---------------------------------------------------------\n");
	__initLog(ELogLevel_info);
	ConsoleApp* app = new ConsoleApp();
	__TestTcpAsyncSvrLogic* logic = new __TestTcpAsyncSvrLogic();
	app->run(logic);
	delete logic;
	delete app;
}



#endif

