#ifndef S_TCP_SOCKET_ASYNC_SERVER_API_H_
#define S_TCP_SOCKET_ASYNC_SERVER_API_H_
#include "../base/iSocketApi.h"
#include "../selector/socketSelector.h"
S_NAMESPACE_BEGIN



class TcpAsyncServerApi : public ITcpAsyncServerApi, private IMessageLoopHandler
{
public:
	TcpAsyncServerApi();
	~TcpAsyncServerApi();
	bool init(MessageLooper* work_looper, void* notify_sender);
	virtual bool atcp_createSvrListenSocket(socket_id_t * svr_listen_sid, const TcpSvrListenSocketCreateParam & param) override;
	virtual void atcp_releaseSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual bool atcp_startSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual void atcp_stopSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual void atcp_stopSvrTranSocket(socket_id_t svr_tran_sid) override;
	virtual bool atcp_sendDataFromSvrTranSocketToClient(socket_id_t svr_tran_sid, const byte_t * data, size_t data_len) override;



private:
	class __SocketCtx
	{
	public:
		__SocketCtx() {
			m_sid = 0;
			m_ref_listen_sid = 0;
			m_socket = INVALID_SOCKET;
			m_selector_session_id = 0;
		}


		socket_id_t m_sid;
		SOCKET m_socket;
		ETcpSocketType m_socket_type;
		socket_id_t m_ref_listen_sid; // svr_tran ref to listen_sid
		uint64_t m_selector_session_id;
		TcpSvrListenSocketCreateParam m_svr_param;
	};
	typedef std::map<socket_id_t, __SocketCtx*> CtxMap;




	virtual void onMessage(Message * msg, bool * is_handled) override;
	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data) override;
	void __onSocketClosedMsg(Message* msg);
	void __onSvrListenSocketAcceptedMsg(Message* msg);
	void __onSvrTranSocketRecvDataMsg(Message* msg);
	void __onSvrTranSocketSendDataEndMsg(Message* msg);

	void __stopSvrListenSocket(socket_id_t svr_listen_sid);
	void __releaseSvrListenSocket(socket_id_t svr_listen_sid);
	void __stopSvrTranSocket(socket_id_t svr_tran_sid);
	void __releaseSvrTranSocket(socket_id_t svr_tran_sid);
	__SocketCtx* __getSvrListenCtxBySelectorSessionId(uint64_t selector_session_id);
	__SocketCtx* __getSvrListenCtxById(socket_id_t sid);
	__SocketCtx* __getSvrTranCtxById(socket_id_t sid);
	__SocketCtx* __getSvrTranCtxBySelectorSessionId(uint64_t selector_session_id);
	void __postMsgToTarget(Message* msg, __SocketCtx * ctx);


	MessageLooper* m_work_looper; 
	void* m_notify_sender;
	Mutex m_mutex;
	CtxMap m_svr_listen_ctx_map;
	CtxMap m_svr_tran_ctx_map;
	SocketSelector* m_selecotr;
};


S_NAMESPACE_END
#endif

//void __onSvrTransSocketClosedMsg(Message* msg);