#ifndef S_TCP_SOCKET_ASYNC_CLIENT_API_H_
#define S_TCP_SOCKET_ASYNC_CLIENT_API_H_
#include "../base/iSocketApi.h"
#include "../selector/socketSelector.h"
#include "helper/tcpConnector.h"
S_NAMESPACE_BEGIN






class TcpAsyncClientApi : public ITcpAsyncClientApi, private IMessageLoopHandler
{
public:
	TcpAsyncClientApi();
	~TcpAsyncClientApi();
	virtual bool init(MessageLooper* work_looper, void* notify_sender);

	virtual bool atcp_createClientSocket(socket_id_t * client_sid, const TcpClientSocketCreateParam & param) override;
	virtual void atcp_releaseClientSocket(socket_id_t client_sid) override;
	virtual bool atcp_startClientSocket(socket_id_t client_sid, uint64_t* connection_id) override;
	virtual void atcp_stopClientSocket(socket_id_t client_sid, uint64_t connection_id) override;
	virtual bool atcp_sendDataFromClientSocketToSvr(socket_id_t client_sid, uint64_t connection_id, const byte_t * data, size_t data_len) override;






private:
	enum __EConnectState
	{
		__EConnectState_connecting,
		__EConnectState_connected,
		__EConnectState_disconnected,
	};
	class __SocketCtx
	{
	public:
		__SocketCtx() { m_socket = INVALID_SOCKET; m_sid = 0; m_connection_id = 0; m_conn_state = __EConnectState_disconnected; }
		~__SocketCtx() { }

		TcpClientSocketCreateParam m_create_param;
		socket_t m_socket;
		socket_id_t m_sid;
		uint64_t m_connection_id;
		__EConnectState m_conn_state;
	};

	virtual void onMessage(Message * msg, bool * is_handled) override;
	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data) override;
	void __onMsg_ConnectorConnectEnd(Message* msg);
	void __onMsg_ClientDisconnected(Message * msg);
	void __onMsg_ClientRecvData(Message* msg);
	void __onMsg_ClientSendDataEnd(Message* msg);

	void __releaseClientSocket(socket_id_t client_sid);
	void __stopClientSocket(socket_id_t client_sid, uint64_t connection_id);
	void __stopClientSocket(__SocketCtx* ctx);
	__SocketCtx* __getClientCtxBySid(socket_id_t sid);
	__SocketCtx* __getClientCtxBySelectorSessionId(uint64_t selector_session_id);
	void __postMsgToTarget(Message* msg, __SocketCtx * ctx);

	Mutex m_mutex;
	MessageLooper* m_work_looper;
	void* m_notify_sender;
	TcpConnector* m_connector;
	SocketSelector* m_selector;
	std::map<socket_id_t, __SocketCtx*> m_ctxs;
};




S_NAMESPACE_END
#endif


