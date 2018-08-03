#include "TcpSyncApi.h"
#include "../util/socketUtil.h"
S_NAMESPACE_BEGIN



TcpSyncApi::TcpSyncApi()
{
	slog_d("TcpSyncApi:: new=%0", (uint64_t)this);
}

TcpSyncApi::~TcpSyncApi()
{
	slog_d("TcpSyncApi:: delete=%0", (uint64_t)this);
	for (SidToSocketMap::iterator it = m_sid_2_socket.begin(); it != m_sid_2_socket.end(); ++it)
	{
		SocketUtil::closeSocket(it->second);
	}
}

bool TcpSyncApi::tcp_open(socket_id_t* sid)
{
	socket_t s = SocketUtil::openSocket(ESocketType_tcp);
	if (s == INVALID_SOCKET)
		return false;

	ScopeMutex __l(m_mutex);
	*sid = SocketUtil::genSid();
	m_sid_2_socket[*sid] = s;
	return true;
}

void TcpSyncApi::tcp_close(socket_id_t sid)
{
	if (sid <= 0)
		return;

	ScopeMutex __l(m_mutex);
	auto it = m_sid_2_socket.find(sid);
	if (it == m_sid_2_socket.end())
		return;

	socket_t s = it->second;
	m_sid_2_socket.erase(it);
	SocketUtil::closeSocket(s);
}

bool TcpSyncApi::tcp_bindAndListen(socket_id_t svr_listen_sid, const std::string& svr_ip_or_name, int svr_port)
{
	if (svr_listen_sid <= 0)
	{
		printf("sid <= 0\n");
		return false;
	}

	socket_t svr_listen_socket = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(svr_listen_sid, &svr_listen_socket))
		{
			printf("!__getSocketBySid\n");
			return false;
		}
	}

	if (!SocketUtil::bindAndListen(svr_listen_socket, svr_ip_or_name, svr_port))
	{
		printf("!SocketUtil::bindAndListen\n");
		return false;
	}

	return true;
}

bool TcpSyncApi::tcp_accept(socket_id_t svr_listen_sid, socket_id_t* svr_tran_sid)
{
	if (svr_listen_sid <= 0 || svr_tran_sid == NULL)
	{
		printf("svr_listen_sid <= 0 || svr_tran_sid == NULL\n");
		return false;
	}
	*svr_tran_sid = INVALID_SOCKET;

	socket_t svr_listen_socket = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(svr_listen_sid, &svr_listen_socket))
		{
			printf("!__getSocketBySid\n");
			return false;
		}
	}

	socket_t svr_tran_socket = INVALID_SOCKET;
	if (!SocketUtil::accept(svr_listen_socket, &svr_tran_socket))
	{
		printf("!SocketUtil::accept\n");
		return false;
	}

	*svr_tran_sid = SocketUtil::genSid();
	m_sid_2_socket[*svr_tran_sid] = svr_tran_socket;

	return true;
}

bool TcpSyncApi::tcp_connect(socket_id_t client_sid, const std::string& svr_ip_or_name, int svr_port)
{
	if (client_sid <= 0)
		return false;

	socket_t client_socket = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(client_sid, &client_socket))
			return false;
	}

	bool ret = SocketUtil::connect(client_socket, svr_ip_or_name, svr_port);
	return ret;
}

void TcpSyncApi::tcp_disconnect(socket_id_t client_sid)
{
	socket_t client_socket = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(client_sid, &client_socket))
			return;
	}

	SocketUtil::disconnect(client_socket);
}

bool TcpSyncApi::tcp_send(socket_id_t sid, const byte_t* data, size_t data_len)
{
	if (sid == 0 || data == NULL || data_len == 0)
		return false;

	socket_t s = 0;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(sid, &s))
			return false;
	}

	return SocketUtil::send(s, data, data_len);
}

bool TcpSyncApi::tcp_recv(socket_id_t sid, byte_t* buf, size_t buf_len, size_t* recv_len)
{
	*recv_len = 0;
	socket_t s = 0;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(sid, &s))
			return false;
	}

	return SocketUtil::recv(s, buf, buf_len, recv_len);
}

bool TcpSyncApi::__getSocketBySid(socket_id_t sid, socket_t* s)
{
	auto it = m_sid_2_socket.find(sid);
	if (it == m_sid_2_socket.end())
		return false;

	*s = it->second;
	return true;
}




S_NAMESPACE_END

