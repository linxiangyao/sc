#ifndef TEST_TCP_CONNECTOR_H
#define TEST_TCP_CONNECTOR_H
#include "../comm.h"
#include "../../src/socket/socketLib.h"
using namespace std;
USING_NAMESPACE_S



class __TestTcpConnectorLogic : public IConsoleAppLogic
{
private:
	virtual void onAppStartMsg(IConsoleAppApi * api) override
	{
		m_api = api;

		initSocketLib();

		m_connector = new TcpConnector();
		MessageLooper* loop = &m_api->getMessageLooper();
		if (!m_connector->init(loop, loop, this))
		{
			slog_e("fail to init");
			return;
		}

		for (uint32_t i = 0; i < 100; ++i)
		{
			__connect(SocketUtil::strToIp("127.0.0.1"), 9901 + i);
		}
	}

	bool __connect(Ip svr_ip, uint32_t svr_port)
	{
		socket_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			slog_e("startClientSocket fail to create socket, s=%0", s);
			return false;
		}

		m_sockets.push_back(s);

		if (!m_connector->startToConnectTcpSocket(s, svr_ip, svr_port))
			return false;

		return true;
	}

	virtual void onAppStopMsg() override
	{
		slog_i("onAppStopMsg begin");
		delete m_connector;
		for (size_t i = 0; i < m_sockets.size(); ++i)
		{
			SocketUtil::closeSocket(m_sockets[i]);
		}
		slog_i("onAppStopMsg end");
	}

	virtual void onMessage(Message * msg, bool * is_handled) override
	{
		if (msg->m_sender == m_connector && msg->m_msg_type == TcpConnector::EMsgType_connecteEnd)
		{
			*is_handled = true;
			TcpConnector::Msg_ConnecteEnd* m = (TcpConnector::Msg_ConnecteEnd*)msg;
			slog_i("Msg_ResolveEnd socket=%0, is_connencted=%1", m->m_socket, m->m_is_connected);
		}
	}



	TcpConnector* m_connector;
	IConsoleAppApi* m_api;

	std::vector<socket_t> m_sockets;
};

void __testTcpConnector()
{
	__initLog(ELogLevel_debug);
	ConsoleApp* app = new ConsoleApp();
	__TestTcpConnectorLogic* logic = new __TestTcpConnectorLogic();
	app->run(logic);
	delete logic;
	delete app;
}




#endif
