#ifndef S_UDP_SOCKET_ASYNC_API_H_
#define S_UDP_SOCKET_ASYNC_API_H_
#include "../base/iSocketApi.h"
#include "../selector/socketSelector.h"
S_NAMESPACE_BEGIN



class UdpAsyncApi : public IUdpAsyncApi, private IMessageLoopHandler
{
public:
	UdpAsyncApi();
	~UdpAsyncApi();
	bool init(MessageLooper* work_looper, void* notify_sender);
	virtual bool audp_createSocket(socket_id_t * sid, const UdpSocketCreateParam & param) override;
	virtual void audp_releaseSocket(socket_id_t sid) override;
	virtual bool audp_sendTo(socket_id_t sid, uint64_t send_cmd_id, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port) override;




private:
	class __SocketCtx
	{
	public:
		__SocketCtx() { m_socket = INVALID_SOCKET; m_sid = 0; }
		~__SocketCtx() { }

		UdpSocketCreateParam m_create_param;
		socket_t m_socket;
		socket_id_t m_sid;
	};


	virtual void onMessage(Message * msg, bool * is_handled) override;
	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data) override;
	void __onMsg_SocketClosed(Message* msg);
	void __onMsg_SocketRecvData(Message* msg);
	void __onMsg_SocketSendDataEnd(Message* msg);
	void __releaseSocket(socket_id_t sid);
	__SocketCtx* __getSocketCtxBySocket(socket_t socket);
	void __postMessageToTarget(Message* msg, __SocketCtx* ctx);


	MessageLooper * m_work_looper;
	void* m_notify_sender;
	Mutex m_mutex;
	SocketSelector* m_selector;
	std::map<socket_id_t, __SocketCtx*> m_ctxs;
};

S_NAMESPACE_END
#endif


