#include "tcpAsyncServerApi.h"
#include "../util/socketUtil.h"
#include "../selector/socketSelector.h"
S_NAMESPACE_BEGIN


TcpAsyncServerApi::TcpAsyncServerApi() 
{
	slog_d("TcpAsyncServerApi:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
	m_selecotr = nullptr;
}

TcpAsyncServerApi::~TcpAsyncServerApi() 
{
	slog_d("TcpAsyncServerApi:: delete=%0", (uint64_t)this);
	m_work_looper->removeMsgHandler(this);

	while (m_svr_listen_ctx_map.size() > 0)
	{
		__releaseSvrListenSocket(m_svr_listen_ctx_map.begin()->second->m_sid);
	}
	while (m_svr_tran_ctx_map.size() > 0)
	{
		__releaseSvrTranSocket(m_svr_tran_ctx_map.begin()->second->m_sid);
	}

	delete m_selecotr;
}

bool TcpAsyncServerApi::init(MessageLooper * work_looper, void* notify_sender)
{
	ScopeMutex __l(m_mutex);
	m_work_looper = work_looper;
	m_notify_sender = notify_sender;

	{
		m_selecotr = new SocketSelector();
		MessageLooperNotifyParam np;
		np.m_notify_looper = m_work_looper;
		np.m_notify_target = this;
		np.m_notify_sender = m_selecotr;
		if (!m_selecotr->init(m_work_looper, np))
		{
			slog_e("TcpAsyncServerApi:: init fail to m_selecotr->init");
			return false;
		}
		if (!m_selecotr->start())
		{
			return false;
		}
	}
	m_work_looper->addMsgHandler(this);
	return true;
}

bool TcpAsyncServerApi::atcp_createSvrListenSocket(socket_id_t * svr_listen_sid, const TcpSvrListenSocketCreateParam & param)
{
	if (svr_listen_sid == nullptr)
		return false;
	*svr_listen_sid = 0;

	if (param.m_notify_looper == nullptr || param.m_svr_ip.size() == 0 || param.m_svr_port == 0)
	{
		slog_e("TcpAsyncServerApi:: createSvrListenSocket fail, param err");
		return false;
	}

	ScopeMutex __l(m_mutex);
	if (m_work_looper == nullptr) // no init
	{
		slog_e("TcpAsyncServerApi:: createSvrListenSocket fail, m_work_looper == nullptr");
		return false;
	}

	__SocketCtx* ctx = new __SocketCtx();
	ctx->m_sid = SocketUtil::genSid();
	ctx->m_socket_type = ETcpSocketType_svr_listen;
	ctx->m_svr_param = param;
	m_svr_listen_ctx_map[ctx->m_sid] = ctx;

	*svr_listen_sid = ctx->m_sid;
	slog_d("TcpAsyncServerApi:: createSvrListenSocket ok, svr_listen_sid=%0", *svr_listen_sid);
	return true;
}

void TcpAsyncServerApi::atcp_releaseSvrListenSocket(socket_id_t svr_listen_sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncServerApi:: atcp_releaseSvrListenSocket svr_listen_sid=%0", svr_listen_sid);
	__releaseSvrListenSocket(svr_listen_sid);
}

bool TcpAsyncServerApi::atcp_startSvrListenSocket(socket_id_t svr_listen_sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncServerApi:: atcp_startSvrListenSocket svr_listen_sid=%0", svr_listen_sid);
	__SocketCtx* ctx = __getSvrListenCtxById(svr_listen_sid);
	if (ctx == nullptr)
	{
		slog_e("TcpAsyncServerApi:: atcp_startSvrListenSocket fail, ctx == nullptr, svr_listen_sid=%0", svr_listen_sid);
		return false;
	}
	if (ctx->m_socket != INVALID_SOCKET)
	{
		slog_d("TcpAsyncServerApi:: atcp_startSvrListenSocket ctx->m_socket != INVALID_SOCKET, already start? ignore. svr_listen_sid=%0", svr_listen_sid);
		return true;
	}

	socket_t s = SocketUtil::openSocket(ESocketType_tcp);
	if (s == INVALID_SOCKET)
	{
		slog_e("TcpAsyncServerApi:: atcp_startSvrListenSocket fail to create socket, svr_listen_sid=%0", svr_listen_sid);
		return false;
	}
	ctx->m_socket = s;
	
	if (!SocketUtil::bindAndListen(s, ctx->m_svr_param.m_svr_ip, ctx->m_svr_param.m_svr_port))
	{
		slog_e("TcpAsyncServerApi:: atcp_startSvrListenSocket fail to bindAndListen, svr_listen_sid=%0", svr_listen_sid);
		// TODO:
		return false;
	}

	if (!m_selecotr->addTcpSocket(s, ETcpSocketType_svr_listen, ctx->m_sid))
	{
		slog_e("TcpAsyncServerApi:: atcp_startSvrListenSocket fail to m_selecotr->addTcpSocket, svr_listen_sid=%0", svr_listen_sid);
		// TODO:
		return false;
	}

	return true;
}

void TcpAsyncServerApi::atcp_stopSvrListenSocket(socket_id_t svr_listen_sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncServerApi:: atcp_stopSvrListenSocket svr_listen_sid=%0", svr_listen_sid);
	__stopSvrListenSocket(svr_listen_sid);
}

void TcpAsyncServerApi::atcp_stopSvrTranSocket(socket_id_t svr_tran_sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncServerApi:: atcp_stopSvrTranSocket svr_tran_sid=%0", svr_tran_sid);
	__stopSvrTranSocket(svr_tran_sid);
}

bool TcpAsyncServerApi::atcp_sendDataFromSvrTranSocketToClient(socket_id_t svr_tran_sid, const byte_t * data, size_t data_len)
{
	ScopeMutex __l(m_mutex);
	slog_v("TcpAsyncServerApi:: atcp_sendDataFromSvrTranSocketToClient send data to client svr_tran_sid=%0, len=%1", svr_tran_sid, data_len);

	__SocketCtx* ctx = __getSvrTranCtxById(svr_tran_sid);
	if (ctx == nullptr)
	{
		slog_d("TcpAsyncServerApi:: atcp_sendDataFromSvrTranSocketToClient fail, maybe closed? ignore, client svr_tran_sid=%0", svr_tran_sid);
		return false;
	}

	m_selecotr->postSendCmd(ctx->m_socket, ctx->m_sid, SocketSelector::genSendCmdId(), data, data_len);
	return true;
}









void TcpAsyncServerApi::onMessage(Message * msg, bool * is_handled)
{
	if (msg->m_target == this)
	{
		if (msg->m_sender == m_selecotr && msg->m_msg_type == SocketSelector::EMsgType_acceptOk)
		{
			__onSvrListenSocketAcceptedMsg(msg);
		}
		else if (msg->m_sender == m_selecotr && msg->m_msg_type == SocketSelector::EMsgType_socketClosed)
		{
			__onSocketClosedMsg(msg);
		}
		else if (msg->m_sender == m_selecotr && msg->m_msg_type == SocketSelector::EMsgType_recvOk)
		{
			__onSvrTranSocketRecvDataMsg(msg);
		}
		else if (msg->m_sender == m_selecotr && msg->m_msg_type == SocketSelector::EMsgType_sendEnd)
		{
			__onSvrTranSocketSendDataEndMsg(msg);
		}
	}
}

void TcpAsyncServerApi::onMessageTimerTick(uint64_t timer_id, void * user_data)
{
}

void TcpAsyncServerApi::__onSvrListenSocketAcceptedMsg(Message * msg)
{
	slog_d("TcpAsyncServerApi:: accept one client");

	SocketSelector::Msg_acceptOk* msg_accept = (SocketSelector::Msg_acceptOk*)msg;
	socket_t svr_listen_socket = msg_accept->m_listen_socket;
	socket_t svr_tran_socket = msg_accept->m_tran_socket;
	__SocketCtx* ctx_listen = __getSvrListenCtxBySocket(svr_listen_socket);
	if (ctx_listen == nullptr)
		return;

	__SocketCtx* ctx_tran = new __SocketCtx();
	ctx_tran->m_sid = SocketUtil::genSid();
	ctx_tran->m_socket_type = ETcpSocketType_svr_tran;
	ctx_tran->m_socket = svr_tran_socket;
	ctx_tran->m_svr_param = ctx_listen->m_svr_param;
	ctx_tran->m_ref_listen_sid = ctx_listen->m_sid;
	m_svr_tran_ctx_map[ctx_tran->m_sid] = ctx_tran;
	
	m_selecotr->addTcpSocket(svr_tran_socket, ETcpSocketType_svr_tran, ctx_tran->m_sid);

	Msg_TcpSvrListenSocketAccepted* m = new Msg_TcpSvrListenSocketAccepted();
	m->m_svr_listen_sid = ctx_listen->m_sid;
	m->m_svr_trans_sid = ctx_tran->m_sid;
	__postMsgToTarget(m, ctx_tran);

	slog_d("TcpAsyncServerApi:: accept client, listen_sid=%0, listen_socket=%1, tran_sid=%2, tran_socket=%3", ctx_listen->m_sid, ctx_listen->m_socket, ctx_tran->m_sid, ctx_tran->m_socket);
}

void TcpAsyncServerApi::__onSocketClosedMsg(Message * msg) 
{
	ScopeMutex __l(m_mutex);
	SocketSelector::Msg_socketClosed* msg_close = (SocketSelector::Msg_socketClosed*)msg;
	__SocketCtx* ctx_listen = __getSvrListenCtxBySocket(msg_close->m_socket);
	if (ctx_listen != nullptr)
	{
		Msg_TcpSvrListenSocketStopped* m = new Msg_TcpSvrListenSocketStopped();
		m->m_svr_listen_sid = ctx_listen->m_sid;
		__postMsgToTarget(m, ctx_listen);
		slog_v("TcpAsyncServerApi:: acceptor close, listen_sid=%0", ctx_listen->m_sid);
		return;
	}

	__SocketCtx* ctx_tran = __getSvrTranCtxBySocket(msg_close->m_socket);
	if (ctx_tran != nullptr)
	{
		Msg_TcpSvrTranSocketStopped* m = new Msg_TcpSvrTranSocketStopped();
		m->m_svr_listen_sid = ctx_tran->m_ref_listen_sid;
		m->m_svr_trans_sid = ctx_tran->m_sid;
		__postMsgToTarget(m, ctx_tran);

		slog_d("TcpAsyncServerApi:: client closed, svr_tran_sid=%0", ctx_tran->m_sid);
		m_selecotr->removeSocket(ctx_tran->m_socket, ctx_tran->m_sid);
		__releaseSvrTranSocket(ctx_tran->m_sid); // api release tran ctx when it is closed
	}

}

void TcpAsyncServerApi::__onSvrTranSocketRecvDataMsg(Message * msg)
{
	ScopeMutex __l(m_mutex);
	SocketSelector::Msg_RecvOk* msg_recv = (SocketSelector::Msg_RecvOk*)msg;
	socket_t svr_tran_socket = msg_recv->m_tran_socket;
	__SocketCtx* ctx_tran = __getSvrTranCtxBySocket(svr_tran_socket);
	if (ctx_tran == nullptr)
		return;

	Msg_TcpSvrTranSocketRecvData* m = new Msg_TcpSvrTranSocketRecvData();
	msg_recv->m_recv_data.detachTo(&m->m_data);
	m->m_svr_listen_sid = ctx_tran->m_ref_listen_sid;
	m->m_svr_trans_sid = ctx_tran->m_sid;
	__postMsgToTarget(m, ctx_tran);
	slog_v("TcpAsyncServerApi:: recv data from client, svr_tran_sid=%0, len=%1", ctx_tran->m_sid, m->m_data.getLen());
}

void TcpAsyncServerApi::__onSvrTranSocketSendDataEndMsg(Message * msg)
{
	ScopeMutex __l(m_mutex);
	SocketSelector::Msg_sendEnd* msg_send_end = (SocketSelector::Msg_sendEnd*)msg;
	socket_t svr_tran_socket = msg_send_end->m_tran_socket;
	__SocketCtx* ctx_tran = __getSvrTranCtxBySocket(svr_tran_socket);
	if (ctx_tran == nullptr)
		return;

	Msg_TcpSvrTranSocketSendDataEnd* m = new Msg_TcpSvrTranSocketSendDataEnd();
	m->m_svr_listen_sid = ctx_tran->m_ref_listen_sid;
	m->m_svr_trans_sid = ctx_tran->m_sid;
	__postMsgToTarget(m, ctx_tran);
	slog_v("TcpAsyncServerApi:: send data to client end, svr_tran_sid=%0", ctx_tran->m_sid);
}

void TcpAsyncServerApi::__stopSvrListenSocket(socket_id_t svr_listen_sid)
{
	__SocketCtx* ctx = __getSvrListenCtxById(svr_listen_sid);
	if (ctx == nullptr || ctx->m_socket == INVALID_SOCKET)
		return;
	m_selecotr->removeSocket(ctx->m_socket, ctx->m_sid);
	SocketUtil::closeSocket(ctx->m_socket);
	ctx->m_socket = INVALID_SOCKET;
}

void TcpAsyncServerApi::__releaseSvrListenSocket(socket_id_t svr_listen_sid)
{
	__stopSvrListenSocket(svr_listen_sid);
	delete_and_erase_map_element_by_key(&m_svr_listen_ctx_map, svr_listen_sid);
}

void TcpAsyncServerApi::__stopSvrTranSocket(socket_id_t svr_tran_sid)
{
	__SocketCtx* ctx = __getSvrTranCtxById(svr_tran_sid);
	if (ctx == nullptr || ctx->m_socket == INVALID_SOCKET)
		return;

	m_selecotr->removeSocket(ctx->m_socket, ctx->m_sid);
	SocketUtil::closeSocket(ctx->m_socket);
	ctx->m_socket = INVALID_SOCKET;
}

void TcpAsyncServerApi::__releaseSvrTranSocket(socket_id_t svr_tran_sid)
{
	__stopSvrTranSocket(svr_tran_sid);
	delete_and_erase_map_element_by_key(&m_svr_tran_ctx_map, svr_tran_sid);
}

TcpAsyncServerApi::__SocketCtx * TcpAsyncServerApi::__getSvrListenCtxBySocket(socket_t socket)
{
	for (auto it = m_svr_listen_ctx_map.begin(); it != m_svr_listen_ctx_map.end(); ++it)
	{
		if (it->second->m_socket == socket)
			return it->second;
	}
	return nullptr; 
}

TcpAsyncServerApi::__SocketCtx * TcpAsyncServerApi::__getSvrListenCtxById(socket_id_t sid)
{ 
	return get_map_element_by_key(m_svr_listen_ctx_map, sid);
}

TcpAsyncServerApi::__SocketCtx * TcpAsyncServerApi::__getSvrTranCtxById(socket_id_t sid)
{
	return get_map_element_by_key(m_svr_tran_ctx_map, sid);
}

TcpAsyncServerApi::__SocketCtx * TcpAsyncServerApi::__getSvrTranCtxBySocket(socket_t socket) 
{
	for (auto it = m_svr_tran_ctx_map.begin(); it != m_svr_tran_ctx_map.end(); ++it)
	{
		if (it->second->m_socket == socket)
			return it->second;
	}
	return nullptr;
}

void TcpAsyncServerApi::__postMsgToTarget(Message * msg, __SocketCtx * ctx)
{
	msg->m_sender = m_notify_sender;
	msg->m_target = ctx->m_svr_param.m_notify_target;
	ctx->m_svr_param.m_notify_looper->postMessage(msg);
}



S_NAMESPACE_END
