#include "udpAsyncApi.h"
#include "../util/socketUtil.h"
#include "../selector/socketSelector.h"
S_NAMESPACE_BEGIN



UdpAsyncApi::UdpAsyncApi()
{
	slog_d("UdpAsyncApi:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
	m_notify_sender = nullptr;
	m_selector = nullptr;
}

UdpAsyncApi::~UdpAsyncApi()
{
	slog_d("UdpAsyncApi:: delete=%0", (uint64_t)this);
	ScopeMutex __l(m_mutex);
	m_work_looper->removeMsgHandler(this);

	while (m_ctxs.size() > 0)
	{
		auto it = m_ctxs.begin();
		__releaseSocket(it->first);
	}
	delete m_selector;
}

bool UdpAsyncApi::init(MessageLooper * work_looper, void * notify_sender)
{
	ScopeMutex __l(m_mutex);
	slog_d("UdpAsyncApi:: init");
	m_work_looper = work_looper;
	m_notify_sender = notify_sender;

	{
		m_selector = new SocketSelector();
		MessageLooperNotifyParam np;
		np.m_notify_looper = m_work_looper;
		np.m_notify_sender = m_selector;
		np.m_notify_target = this;
		if (!m_selector->init(m_work_looper, np))
		{
			slog_e("UdpAsyncApi:: init fail m_selector->init");
			return false;
		}
		if (!m_selector->start())
		{
			slog_e("UdpAsyncApi:: init fail m_selector->start");
			return false;
		}
	}

	m_work_looper->addMsgHandler(this);
	return true; 
}

bool UdpAsyncApi::audp_createSocket(socket_id_t * sid, const UdpSocketCreateParam & param)
{
	ScopeMutex __l(m_mutex);
	socket_t socket = SocketUtil::openSocket(ESocketType_udp);
	if (socket == INVALID_SOCKET)
	{
		slog_e("UdpAsyncApi:: audp_createSocket fail to openSocket");
		return false;
	}

	__SocketCtx* ctx = new __SocketCtx();
	ctx->m_sid = SocketUtil::genSid();
	ctx->m_socket = socket;
	ctx->m_create_param = param;
	ctx->m_selector_session_id = SocketSelector::genSessionId();
	m_ctxs[ctx->m_sid] = ctx;

	if (param.m_is_bind)
	{
		if (!SocketUtil::bind(socket, param.m_bind_ip, param.m_bind_port))
		{
			slog_e("UdpAsyncApi:: audp_createSocket fail to SocketUtil::bind");
			return false;
		}
	}

	if (!m_selector->addUdpSocket(ctx->m_socket, ctx->m_selector_session_id))
	{
		__releaseSocket(ctx->m_sid);
		slog_e("UdpAsyncApi:: audp_createSocket fail to m_selector->addUdpSocket");
		return false;
	}

	*sid = ctx->m_sid;
	slog_d("UdpAsyncApi:: audp_createSocket sid=%0, socket=%1", ctx->m_sid, ctx->m_socket);
	return true;
}

void UdpAsyncApi::audp_releaseSocket(socket_id_t sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("UdpAsyncApi:: audp_releaseSocket sid=%0", sid);
	__releaseSocket(sid);
}

bool UdpAsyncApi::audp_sendTo(socket_id_t sid, uint64_t send_cmd_id, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port)
{
	ScopeMutex __l(m_mutex);
	__SocketCtx* ctx = get_map_element_by_key(m_ctxs, sid);
	if (ctx == nullptr)
		return false;
	if (!m_selector->postSendToCmd(ctx->m_socket, ctx->m_selector_session_id, send_cmd_id, to_ip, to_port, data, data_len))
		return false;
	return true;
}

void UdpAsyncApi::onMessage(Message * msg, bool * is_handled)
{
	if (msg->m_target != this)
		return;

	ScopeMutex __l(m_mutex);
	*is_handled = true;
	switch (msg->m_msg_type)
	{
	case SocketSelector::EMsgType_socketClosed: __onMsg_SocketClosed(msg); break;
	case SocketSelector::EMsgType_recvFromOk:	__onMsg_SocketRecvData(msg); break;
	case SocketSelector::EMsgType_sendToEnd:	__onMsg_SocketSendDataEnd(msg); break;
	}
}

void UdpAsyncApi::onMessageTimerTick(uint64_t timer_id, void * user_data)
{
}

void UdpAsyncApi::__onMsg_SocketClosed(Message * msg)
{
	SocketSelector::Msg_socketClosed* msg_closed = (SocketSelector::Msg_socketClosed*)msg;
	__SocketCtx* ctx = __getSocketCtxBySelectorSessionId(msg_closed->m_session_id);
	if (ctx == nullptr)
		return;

	Msg_UdpSocketClosed* m = new Msg_UdpSocketClosed();
	m->m_sid = ctx->m_sid;
	__postMessageToTarget(m, ctx);
	__releaseSocket(ctx->m_sid);
}

void UdpAsyncApi::__onMsg_SocketRecvData(Message * msg)
{
	SocketSelector::Msg_RecvFromOk* msg_recv_from = (SocketSelector::Msg_RecvFromOk*)msg;
	__SocketCtx* ctx = __getSocketCtxBySelectorSessionId(msg_recv_from->m_session_id);
	if (ctx == nullptr)
		return;

	Msg_UdpSocketRecvData* m = new Msg_UdpSocketRecvData();
	m->m_sid = ctx->m_sid;
	m->m_from_ip = msg_recv_from->m_from_ip;
	m->m_from_port = msg_recv_from->m_from_port;
	m->m_recv_data.attach(&msg_recv_from->m_recv_data);
	__postMessageToTarget(m, ctx);
}

void UdpAsyncApi::__onMsg_SocketSendDataEnd(Message * msg)
{
	SocketSelector::Msg_sendToEnd* msg_send_to_end = (SocketSelector::Msg_sendToEnd*)msg;
	__SocketCtx* ctx = __getSocketCtxBySelectorSessionId(msg_send_to_end->m_session_id);
	if (ctx == nullptr)
		return;

	Msg_UdpSocketSendDataEnd* m = new Msg_UdpSocketSendDataEnd();
	m->m_sid = ctx->m_sid;
	__postMessageToTarget(m, ctx);
}

void UdpAsyncApi::__releaseSocket(socket_id_t sid) 
{
	__SocketCtx* ctx = get_map_element_by_key(m_ctxs, sid);
	if (ctx == nullptr)
		return;

	SocketUtil::closeSocket(ctx->m_socket);
	m_selector->removeSocket(ctx->m_socket, ctx->m_selector_session_id);

	delete_and_erase_map_element_by_key(&m_ctxs, sid);
}

UdpAsyncApi::__SocketCtx * UdpAsyncApi::__getSocketCtxBySelectorSessionId(uint64_t session_id)
{
	for (auto it = m_ctxs.begin(); it != m_ctxs.end(); ++it)
	{
		if (it->second->m_selector_session_id == session_id)
			return it->second;
	}
	return nullptr;
}

void UdpAsyncApi::__postMessageToTarget(Message* msg, __SocketCtx * ctx)
{
	msg->m_sender = m_notify_sender;
	msg->m_target = ctx->m_create_param.m_notify_target;
	ctx->m_create_param.m_notify_looper->postMessage(msg);
}




S_NAMESPACE_END

