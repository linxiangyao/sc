#include "tcpAsyncClientApi.h"
#include "../util/socketUtil.h"
#include "../selector/socketSelector.h"
S_NAMESPACE_BEGIN



TcpAsyncClientApi::TcpAsyncClientApi()
{
	slog_d("TcpAsyncClientApi:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
	m_connector = nullptr;
}

TcpAsyncClientApi::~TcpAsyncClientApi()
{
	slog_d("TcpAsyncClientApi:: delete=%0", (uint64_t)this);

	ScopeMutex __l(m_mutex);

	while (m_ctxs.size() > 0)
	{
		__releaseClientSocket(m_ctxs.begin()->second->m_sid);
	}

	m_work_looper->removeMsgHandler(this);
	delete m_connector;
	delete m_selector;
}

bool TcpAsyncClientApi::init(MessageLooper* work_looper, void* notify_sender)
{
	slog_d("TcpAsyncClientApi:: init");
	ScopeMutex __l(m_mutex);
	m_work_looper = work_looper;
	m_work_looper->addMsgHandler(this);
	m_notify_sender = notify_sender;

	m_connector = new TcpConnector();
	if (!m_connector->init(m_work_looper, m_work_looper, this))
	{
		slog_e("TcpAsyncClientApi:: init fail to m_connector->init");
		return false;
	}

	{
		m_selector = new SocketSelector();
		MessageLooperNotifyParam np;
		np.m_notify_looper = m_work_looper;
		np.m_notify_sender = m_selector;
		np.m_notify_target = this;
		if (!m_selector->init(m_work_looper, np))
		{
			slog_e("TcpAsyncClientApi:: init fail to m_selector->init");
			return false;
		}
		if (!m_selector->start())
		{
			slog_e("TcpAsyncClientApi:: init fail to m_selector->start");
			return false;
		}
	}

	return true;
}

bool TcpAsyncClientApi::atcp_createClientSocket(socket_id_t* client_sid, const TcpClientSocketCreateParam & param)
{
	if (client_sid == NULL)
		return false;
	*client_sid = 0;

	if (param.m_notify_looper == NULL || param.m_notify_target == NULL || param.m_svr_ip.m_type == EIpType_none || param.m_svr_port == 0)
	{
		slog_e("TcpAsyncClientApi:: atcp_createClientSocket fail, param error");
		return false;
	}

	ScopeMutex __l(m_mutex);
	if (m_work_looper == NULL) // no init
	{
		slog_e("TcpAsyncClientApi:: atcp_createClientSocket fail, m_work_looper == NULL");
		return false;
	}

	__SocketCtx* ctx = new __SocketCtx();
	ctx->m_sid = SocketUtil::genSid();
	ctx->m_create_param = param;
	m_ctxs[ctx->m_sid] = ctx;

	*client_sid = ctx->m_sid;

	slog_d("TcpAsyncClientApi::atcp_createClientSocket ok, client_sid=%0", *client_sid);
	return true;
}

void TcpAsyncClientApi::atcp_releaseClientSocket(socket_id_t client_sid)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncClientApi:: atcp_releaseClientSocket client_sid=%0", client_sid);
	__releaseClientSocket(client_sid);
}

bool TcpAsyncClientApi::atcp_startClientSocket(socket_id_t client_sid, uint64_t* connection_id)
{
	ScopeMutex __l(m_mutex);
	__SocketCtx* ctx = __getClientCtxBySid(client_sid);
	if (ctx == NULL)
	{
		slog_e("TcpAsyncClientApi:: atcp_startClientSocket fail to find ctx, cient_sid=%0", client_sid);
		return false;
	}
	if (ctx->m_socket != INVALID_SOCKET)
	{
		slog_d("TcpAsyncClientApi:: atcp_startClientSocket ctx->m_socket != INVALID_SOCKET, already start? ignore, cient_sid=%0", client_sid);
		*connection_id = ctx->m_connection_id;
		return true;
	}

	socket_t s = SocketUtil::openSocket(ESocketType_tcp);
	if (s == INVALID_SOCKET)
	{
		slog_e("TcpAsyncClientApi:: atcp_startClientSocket fail to create socket, cient_sid=%0", client_sid);
		return false;
	}
	ctx->m_socket = s;
	ctx->m_connection_id = SocketSelector::genSessionId();
	*connection_id = ctx->m_connection_id;

	if (!m_connector->startToConnectTcpSocket(ctx->m_socket, ctx->m_create_param.m_svr_ip, ctx->m_create_param.m_svr_port))
		return false;
	ctx->m_conn_state = __EConnectState_connecting;

	slog_d("TcpAsyncClientApi:: atcp_startClientSocket cient_sid=%0, connection_id=%1", client_sid, *connection_id);
	return true;
}

void TcpAsyncClientApi::atcp_stopClientSocket(socket_id_t client_sid, uint64_t connection_id)
{
	ScopeMutex __l(m_mutex);
	slog_d("TcpAsyncClientApi:: atcp_stopClientSocket cient_sid=%0, connection_id=%1", client_sid, connection_id);
	__stopClientSocket(client_sid, connection_id);
}

bool TcpAsyncClientApi::atcp_sendDataFromClientSocketToSvr(socket_id_t client_sid, uint64_t connection_id, const byte_t* data, size_t data_len)
{
	ScopeMutex __l(m_mutex);
	slog_v("TcpAsyncClientApi:: atcp_sendDataFromClientSocketToSvr send data to svr client_sid=%0, connection_id=%1", client_sid, connection_id);
	__SocketCtx* ctx = __getClientCtxBySid(client_sid);
	if (ctx == NULL || connection_id != ctx->m_connection_id)
	{
		slog_w("TcpAsyncClientApi:: atcp_sendDataFromClientSocketToSvr fail to find ctx, maybe closed? ignore, client_sid=%0, connection_id=%1", client_sid, connection_id);
		return false;
	}

	if (!m_selector->postSendCmd(ctx->m_socket, ctx->m_connection_id, SocketSelector::genSendCmdId(), data, data_len))
	{
		slog_e("TcpAsyncClientApi:: atcp_sendDataFromClientSocketToSvr fail to postSendCmd, client_sid=%0, connection_id=%1", client_sid, connection_id);
		return false;
	}

	return true;
}







void TcpAsyncClientApi::onMessage(Message * msg, bool * is_handled)
{
	if (msg->m_target != this)
		return;

	ScopeMutex __l(m_mutex);
	*is_handled = true;
	switch (msg->m_msg_type)
	{
	case TcpConnector::EMsgType_connecteEnd:	__onMsg_ConnectorConnectEnd(msg); break;
	case SocketSelector::EMsgType_socketClosed: __onMsg_ClientDisconnected(msg); break;
	case SocketSelector::EMsgType_recvOk:		__onMsg_ClientRecvData(msg); break;
	case SocketSelector::EMsgType_sendEnd:		__onMsg_ClientSendDataEnd(msg); break;
	}
}

void TcpAsyncClientApi::onMessageTimerTick(uint64_t timer_id, void * user_data)
{
}

void TcpAsyncClientApi::__onMsg_ConnectorConnectEnd(Message * msg)
{
	TcpConnector::Msg_ConnecteEnd* msg_connect = (TcpConnector::Msg_ConnecteEnd*)msg;
	__SocketCtx* ctx = nullptr;
	for (auto it = m_ctxs.begin(); it != m_ctxs.end(); ++it)
	{
		if (it->second->m_socket == msg_connect->m_socket)
		{
			ctx = it->second;
			break;
		}
	}
	if (ctx == nullptr)
		return;


	if (msg_connect->m_is_connected)
	{
		Msg_TcpClientSocketConnected* m = new Msg_TcpClientSocketConnected();
		m->m_client_sid = ctx->m_sid;
		__postMsgToTarget(m, ctx);
		slog_v("TcpAsyncClientApi:: on client connected msg ok, client_sid=%0", ctx->m_sid);

		m_selector->addTcpSocket(ctx->m_socket, ETcpSocketType_client, ctx->m_connection_id);
	}
	else
	{
		Msg_TcpClientSocketDisconnected* m = new Msg_TcpClientSocketDisconnected();
		m->m_client_sid = ctx->m_sid;
		__postMsgToTarget(m, ctx);
		slog_v("TcpAsyncClientApi:: on client disconnected msg ok, client_sid=%0", ctx->m_sid);
	}
}

void TcpAsyncClientApi::__onMsg_ClientDisconnected(Message * msg)
{
	SocketSelector::Msg_socketClosed* msg_disconnect = (SocketSelector::Msg_socketClosed*)msg;
	socket_t socket = msg_disconnect->m_socket;
	uint64_t connection_id = msg_disconnect->m_session_id;
	__SocketCtx* ctx = __getClientCtxBySocket(socket);
	if (ctx == NULL || connection_id != ctx->m_connection_id)
		return;
	if (ctx->m_socket == INVALID_SOCKET)
		return;
	ctx->m_socket = INVALID_SOCKET;
	ctx->m_connection_id = 0;

	{
		Msg_TcpClientSocketDisconnected* m = new Msg_TcpClientSocketDisconnected();
		m->m_client_sid = ctx->m_sid;
		__postMsgToTarget(m, ctx);
		slog_v("TcpAsyncClientApi:: client disconnected msg ok, client_sid=%0", ctx->m_sid);
	}
}

void TcpAsyncClientApi::__onMsg_ClientRecvData(Message * msg)
{
	SocketSelector::Msg_RecvOk* msg_recv_data = (SocketSelector::Msg_RecvOk*)msg;
	socket_t socket = msg_recv_data->m_socket;
	uint64_t connection_id = msg_recv_data->m_session_id;
	__SocketCtx* ctx = __getClientCtxBySocket(socket);
	if (ctx == NULL || connection_id != ctx->m_connection_id)
		return;

	Msg_TcpClientSocketRecvData* m = new Msg_TcpClientSocketRecvData();
	msg->m_args.getByteArrayAndDetachTo("data", &m->m_recv_data);
	m->m_client_sid = ctx->m_sid;
	__postMsgToTarget(m, ctx);
	slog_v("TcpAsyncClientApi:: on client recv data msg ok, client_sid=%0, len=%1", ctx->m_sid, m->m_recv_data.getLen());
}

void TcpAsyncClientApi::__onMsg_ClientSendDataEnd(Message * msg)
{
	SocketSelector::Msg_sendEnd* msg_send_data_end = (SocketSelector::Msg_sendEnd*)msg;
	socket_t socket = msg_send_data_end->m_socket;
	uint64_t connection_id = msg_send_data_end->m_session_id;
	__SocketCtx* ctx = __getClientCtxBySocket(socket);
	if (ctx == NULL || connection_id != ctx->m_connection_id)
		return;

	Msg_TcpClientSocketSendDataEnd* m = new Msg_TcpClientSocketSendDataEnd();
	m->m_client_sid = ctx->m_sid;
	__postMsgToTarget(m, ctx);
	slog_v("TcpAsyncClientApi:: on client send data end msg ok, client_sid=%0", ctx->m_sid);
}






void TcpAsyncClientApi::__releaseClientSocket(socket_id_t client_sid)
{
	__SocketCtx* ctx = __getClientCtxBySid(client_sid);
	if (ctx == NULL)
		return;
	__stopClientSocket(ctx);
	delete_and_erase_map_element_by_key(&m_ctxs, client_sid);
}

void TcpAsyncClientApi::__stopClientSocket(socket_id_t client_sid, uint64_t connection_id)
{
	__SocketCtx* ctx = __getClientCtxBySid(client_sid);
	if (ctx == NULL || ctx->m_connection_id != connection_id)
		return;
	__stopClientSocket(ctx);
}

void TcpAsyncClientApi::__stopClientSocket(__SocketCtx * ctx)
{
	if (ctx->m_socket == INVALID_SOCKET)
		return;
	m_connector->stopConnectTcpSocket(ctx->m_socket);
	m_selector->removeSocket(ctx->m_socket, ctx->m_connection_id);
	SocketUtil::closeSocket(ctx->m_socket);
	ctx->m_socket = INVALID_SOCKET;
	ctx->m_connection_id = 0;
}

TcpAsyncClientApi::__SocketCtx* TcpAsyncClientApi::__getClientCtxBySid(socket_id_t sid)
{ 
	return get_map_element_by_key(m_ctxs, sid);
}

TcpAsyncClientApi::__SocketCtx* TcpAsyncClientApi::__getClientCtxBySocket(socket_t socket)
{
	for (auto it = m_ctxs.begin(); it != m_ctxs.end(); ++it)
	{
		if (it->second->m_socket == socket)
			return it->second;
	}
	return nullptr;
}

void TcpAsyncClientApi::__postMsgToTarget(Message * msg, __SocketCtx * ctx)
{
	if (ctx == nullptr)
		return;
	msg->m_sender = m_notify_sender;
	msg->m_target = ctx->m_create_param.m_notify_target;
	ctx->m_create_param.m_notify_looper->postMessage(msg);
}





S_NAMESPACE_END

