#ifndef S_SOCKET_SELECTOR_H_
#define S_SOCKET_SELECTOR_H_
#include "../base/iSocketApi.h"
S_NAMESPACE_BEGIN


#ifdef S_OS_WIN
#define S_SELECTOR_IOCP
#elif defined(S_OS_LINUX)
#define S_SELECTOR_EPOLL
#elif defined(S_OS_MAC)
#define S_SELECTOR_KQUEUE
#endif


class SocketSelector
{
public:
	enum EMsgType
	{
		EMsgType_socketClosed = -234215476,
		EMsgType_acceptOk,
		EMsgType_sendEnd,
		EMsgType_sendToEnd,
		EMsgType_recvOk,
		EMsgType_recvFromOk,
	};

	class BaseMsg : public Message
	{
	public:
		BaseMsg() { m_session_id = 0; m_cmd_id = 0; }
		uint64_t m_session_id;
		uint64_t m_cmd_id;
	};

	class Msg_socketClosed : public BaseMsg
	{
	public:
		Msg_socketClosed() { m_msg_type = EMsgType_socketClosed; m_socket = INVALID_SOCKET; }
		socket_t m_socket;
	};

	class Msg_acceptOk : public BaseMsg
	{
	public:
		Msg_acceptOk() { m_msg_type = EMsgType_acceptOk; m_listen_socket = INVALID_SOCKET; m_tran_socket = INVALID_SOCKET; }
		socket_t m_listen_socket;
		socket_t m_tran_socket;
	};

	class Msg_sendEnd : public BaseMsg
	{
	public:
		Msg_sendEnd() { m_msg_type = EMsgType_sendEnd; m_tran_socket = INVALID_SOCKET; m_is_ok = false; }
		Binary m_recv_data;
		socket_t m_tran_socket;
		bool m_is_ok;
	};

	class Msg_sendToEnd : public BaseMsg
	{
	public:
		Msg_sendToEnd() { m_msg_type = EMsgType_sendToEnd; m_tran_socket = INVALID_SOCKET; m_is_ok = false; }
		Binary m_recv_data;
		socket_t m_tran_socket;
		bool m_is_ok;
	};

	class Msg_RecvOk : public BaseMsg
	{
	public:
		Msg_RecvOk() { m_msg_type = EMsgType_recvOk; m_tran_socket = INVALID_SOCKET; }
		Binary m_recv_data;
		socket_t m_tran_socket;
	};

	class Msg_RecvFromOk : public BaseMsg
	{
	public:
		Msg_RecvFromOk() { m_msg_type = EMsgType_recvFromOk; m_tran_socket = INVALID_SOCKET; }
		socket_t m_tran_socket;
		Binary m_recv_data;
		Ip m_from_ip;
		uint32_t m_from_port;
	};



	SocketSelector();
	~SocketSelector();
	bool init(MessageLooper* work_looper, const MessageLooperNotifyParam& notify_param);
	bool start();
	void stop();
	bool addTcpSocket(socket_t socket, ETcpSocketType socket_type, uint64_t session_id);
	bool addUdpSocket(socket_t socket, uint64_t session_id);
	void removeSocket(socket_t socket, uint64_t session_id);
	bool postSendCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, const byte_t* data, size_t data_len);
	bool postSendToCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, Ip to_ip, uint32_t to_port, const byte_t* data, size_t data_len);
	static uint64_t genSendCmdId();
	static uint64_t genSessionId();



private:
	class __WorkRun;
	class __SocketCtx
	{
	public:
		socket_t m_socket;
	};


	bool __addTranSocket(socket_t socket, uint64_t connection_id, bool is_tcp);
	bool __addAccpetSocket(socket_t socket, uint64_t connection_id);

	MessageLooper* m_work_looper;
	MessageLooperNotifyParam m_notify_param;
	Mutex m_mutex;
	bool m_is_exit;
	std::map<socket_t, Thread*> m_listen_threads;
	std::vector<Thread*> m_tran_threads;
	std::map<socket_t, __SocketCtx*> m_tran_sockets;
#if defined(S_SELECTOR_IOCP)
	HANDLE m_completionPort;
#endif // S_OS_WIN
};




S_NAMESPACE_END
#endif
