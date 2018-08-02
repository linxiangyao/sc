#include "udpSyncApi.h"
#include "../util/socketUtil.h"
S_NAMESPACE_BEGIN




bool UdpSyncApi::udp_open(socket_id_t * sid)
{
	socket_t s = SocketUtil::openSocket(ESocketType_udp);
	if (s == INVALID_SOCKET)
		return false;

	ScopeMutex __l(m_mutex);
	*sid = SocketUtil::genSid();
	m_sid_2_socket[*sid] = s;
	return true;
}

void UdpSyncApi::udp_close(socket_id_t sid)
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

bool UdpSyncApi::udp_sendTo(socket_id_t sid, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port)
{
	if (sid == 0 || data == NULL || data_len == 0)
		return false;

	socket_t s = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(sid, &s))
			return false;
	}

	return SocketUtil::sendTo(s, data, data_len, to_ip, to_port);
}

bool UdpSyncApi::udp_recvFrom(socket_id_t sid, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len)
{
	*real_recv_len = 0;
	socket_t s = INVALID_SOCKET;
	{
		ScopeMutex __l(m_mutex);
		if (!__getSocketBySid(sid, &s))
			return false;
	}

	return SocketUtil::recvFrom(s, buf, buf_len, from_ip, from_port, real_recv_len);
}

bool UdpSyncApi::__getSocketBySid(socket_id_t sid, socket_t* s)
{
	auto it = m_sid_2_socket.find(sid);
	if (it == m_sid_2_socket.end())
		return false;

	*s = it->second;
	return true;
}



S_NAMESPACE_END

