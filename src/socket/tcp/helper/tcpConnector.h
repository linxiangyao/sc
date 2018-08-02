#ifndef S_TCP_CONNECTOR_H_
#define S_TCP_CONNECTOR_H_
#include "../../base/iSocketApi.h"
S_NAMESPACE_BEGIN


/*
connect tcp socket, and notify message
*/

class TcpConnector : public IMessageHandler
{
public:
	enum EMsgType
	{
		EMsgType_connecteEnd = -3427651,
	};

	class Msg_ConnecteEnd : public Message
	{
	public:
		Msg_ConnecteEnd(socket_t s, bool is_connected)
		{
			m_msg_type = EMsgType_connecteEnd;
			m_socket = s;
			m_is_connected = is_connected;
		}

		socket_id_t m_socket;
		bool m_is_connected;
	};

	class __SvrInfo
	{
	public:
		__SvrInfo() {}
		__SvrInfo(socket_t s, Ip ip, uint32_t port)
		{
			m_socket = s;
			m_ip = ip;
			m_port = port;
		}

		socket_t m_socket;
		Ip m_ip;
		uint32_t m_port;
	};





	TcpConnector();
	~TcpConnector();
	bool init(MessageLooper* work_loop, MessageLooper* notify_loop, void* notify_target);

	virtual void stopTcpConnector();
	virtual bool startToConnectTcpSocket(socket_t s, Ip svr_ip, uint32_t svr_port);
	virtual void stopConnectTcpSocket(socket_t s);





private:
	class __WorkRun;

	virtual void onMessage(Message * msg, bool * is_handled) override;

	void __stop();
	void __doConnect();
	void __notifyConnectEnd(socket_t s, bool is_connected);

	Mutex m_mutex;
	MessageLooper* m_work_loop;
	MessageLooper* m_notify_loop;
	void* m_notify_target;
	std::vector<Thread*> m_connect_threads;
	std::vector<__SvrInfo*> m_svr_infos;
};




S_NAMESPACE_END
#endif
