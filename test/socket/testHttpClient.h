#ifndef TEST_HTTP_CLIENT_H
#define TEST_HTTP_CLIENT_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S





// callback client --------------------------------------------------------------------------------------------------
class __TestHttpClientogic : public IConsoleAppLogic
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
		__creatHttpReq(textMsg);
	}

	virtual void onMessage(Message * msg, bool* is_handled) override
	{
		if (msg->m_target != this)
			return;

		if (msg->m_sender == m_sapi)
		{
			switch (msg->m_msg_type)
			{
			case ITcpAsyncApi::EMsgType_tcpClientSocketConnected:
				__onMsg_clientConnected(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketDisconnected:
				printf("client:: disconnected\n");
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketRecvData:
				__onMsg_recvData(msg);
				break;

			case ITcpAsyncApi::EMsgType_tcpClientSocketSendDataEnd:
				__onMsg_sendDataEnd(msg);
				break;
			default:
				break;
			}
		}
	}

	void __onMsg_clientConnected(Message* msg)
	{
		printf("client:: connected\n");
		ITcpAsyncClientApi::Msg_TcpClientSocketConnected* msg_connected = (ITcpAsyncClientApi::Msg_TcpClientSocketConnected*)msg;
		__HttpCtx* ctx = __getCtxBySid(msg_connected->m_client_sid);
		if (ctx == nullptr)
			return;

		std::string get_str = std::string() + "GET " + ctx->m_res + " HTTP/1.1\r\n"
			+ "Host: " + ctx->m_host + "\r\n"
			+ "\r\n";
		slog_i("send http get:%0", get_str);
		__sendPackToSvr(ctx->m_sid, ctx->m_connection_id, get_str);
	}

	void __onMsg_recvData(Message* msg)
	{
		ITcpAsyncClientApi::Msg_TcpClientSocketRecvData* m = (ITcpAsyncClientApi::Msg_TcpClientSocketRecvData*)msg;
		if (m->m_recv_data.getLen() > 0)
		{
			const char* sz = (const char*)m->m_recv_data.getData();
			std::string str;
			str.append(sz, m->m_recv_data.getLen());
			FileUtil::writeFile("1.txt", str);
		}
	}

	void __onMsg_sendDataEnd(Message* msg)
	{

	}

	void __onSentPack()
	{
		/*const uint64_t block_size = 1 * 1024 * 1024;
		uint64_t send_block_count = m_upload_pack_count * __TestCallbackClientSpeedLogic_uploadPackSize / block_size;

		while (send_block_count > m_already_print_send_block_count)
		{
			++m_already_print_send_block_count;
			printf(".");
		}*/
	}

	bool __creatHttpReq(const std::string& url)
	{
		std::string url_path = StringUtil::trim(url);
		url_path = StringUtil::trimBegin(url_path, "http://");
		std::string host = url_path;
		std::string res;
		if (url_path.find("/") != std::string::npos)
		{
			host = StringUtil::fetchMiddle(url_path, "", "/");
			res = std::string("/") + StringUtil::fetchMiddle(url_path, "/", "");
		}
		else
		{
			res = "/";
		}

		slog_d("url=%0, host=%1, res=%2", url, host, res);
		if (host.size() == 0)
			return false;
		Ip ip = SocketUtil::resolveIpByName(host);
		if (ip.m_type == EIpType_none)
		{
			slog_e("fail to resolve host");
			return false;
		}

		__HttpCtx* ctx = new __HttpCtx();
		ctx->m_host = host;
		ctx->m_res = res;
		ITcpAsyncClientApi::TcpClientSocketCreateParam param;
		param.m_notify_looper = &m_console_api->getMessageLooper();
		param.m_notify_target = this;
		param.m_svr_ip = ip;
		param.m_svr_port = 80;
		if (!m_sapi->atcp_createClientSocket(&ctx->m_sid, param))
		{
			printf("client:: fail to m_sapi->createClientSocket\n");
			m_console_api->exit();
			return false;
		}

		if (!m_sapi->atcp_startClientSocket(ctx->m_sid, &ctx->m_connection_id))
		{
			printf("client:: fail to m_sapi->startClientSocket\n");
			m_console_api->exit();
			return false;
		}
		m_ctxs.push_back(ctx);
		return true;
	}

	bool __sendPackToSvr(socket_id_t sid, uint64_t connection_id, const std::string& str)
	{
		if (!m_sapi->atcp_sendDataFromClientSocketToSvr(sid, connection_id, (const byte_t*)str.c_str(), str.size() + 1))
		{
			printf("fail to sendDataFromClientSocketToSvr\n");
			return false;
		};
		return true;
	}

	bool __sendPackToSvr(socket_id_t sid, uint64_t connection_id, const byte_t* data, size_t data_len)
	{
		bool is_ok = m_sapi->atcp_sendDataFromClientSocketToSvr(sid, connection_id, data, data_len);
		if (!is_ok)
		{
			printf("fail to __sendPackToSvr\n");
			return false;
		}
		return is_ok;
	}


	class __HttpCtx
	{
	public:
		socket_id_t m_sid;
		uint64_t m_connection_id;
		std::string m_host;
		std::string m_res;
	};

	__HttpCtx* __getCtxBySid(uint64_t sid)
	{
		for (size_t i = 0; i < m_ctxs.size(); ++i)
		{
			if (m_ctxs[i]->m_sid == sid)
				return m_ctxs[i];
		}
		return nullptr;
	}

	IConsoleAppApi* m_console_api;
	SocketApi* m_sapi;
	std::vector<__HttpCtx*> m_ctxs;
};







void __testHttpClient()
{
	printf("\n__testHttpClient ---------------------------------------------------------\n");
	__initLog(ELogLevel_debug);
	ConsoleApp* app = new ConsoleApp();
	__TestHttpClientogic* logic = new __TestHttpClientogic();
	app->run(logic);
	delete logic;
	delete app;
}




#endif

