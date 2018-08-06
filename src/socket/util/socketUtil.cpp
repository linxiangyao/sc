#include "socketUtil.h"
S_NAMESPACE_BEGIN


Mutex SocketUtil::s_sid_mutex;
socket_id_t SocketUtil::m_sid_seed = 0;
uint64_t SocketUtil::m_connection_id_seed = 0;




socket_id_t SocketUtil::genSid()
{
	ScopeMutex __l(s_sid_mutex);
	return ++m_sid_seed;
}
//
//uint64_t SocketUtil::genConnectionId()
//{
//	ScopeMutex __l(s_sid_mutex);
//	return ++m_connection_id_seed;
//}

socket_t SocketUtil::openSocket(ESocketType socket_type)
{
	if (socket_type == ESocketType_tcp)
		return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if (socket_type == ESocketType_udp)
		return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else
		return INVALID_SOCKET;
}

bool SocketUtil::bindAndListen(socket_t s, const std::string& svr_ip, int svr_port)
{
	Ip ip = __resovleIpByName(svr_ip);
	if (ip.m_type == EIpType_none)
	{
		slog_d("SocketUtil::bindAndListen fail __resovleIpByName");
		return false;
	}

	if (!bind(s, ip, svr_port))
		return false;


	if (listen(s, 5) < 0)
	{
		slog_d("SocketUtil::bindAndListen fail to listen! error code : %0", getErr());
		return false;
	}

	return true;
}

bool SocketUtil::bind(socket_t s, Ip ip, int port)
{
	sockaddr_storage addr;
	int addr_len = 0;
	__initAddrStorage(&addr, &addr_len, ip, port);

	int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable)) != 0)
	{
		slog_d("SocketUtil::bindAndListen fail to setsockopt SO_REUSEADDR! error code : %0", getErr());
		return false;
	}

	if (::bind(s, (sockaddr*)&addr, addr_len) < 0)
	{
		slog_d("SocketUtil::bindAndListen fail to bind! error code : %0", getErr());
		return false;
	}

	return true;
}

bool SocketUtil::accept(socket_t svr_accept_socket, socket_t* svr_trans_socket)
{
	sockaddr_storage from_addr;
	memset(&from_addr, 0, sizeof(sockaddr_storage));
	socklen_t addr_len = sizeof(sockaddr_storage);

	*svr_trans_socket = ::accept(svr_accept_socket, (sockaddr*)&from_addr, &addr_len);
	if (*svr_trans_socket == INVALID_SOCKET)
	{
		slog_d("SocketUtil::accept fail to accept");
		return false;
	}

	{
		sockaddr* addr = (sockaddr*)&from_addr;
		std::string remote_ip_str;
		ipToStr(addr, &remote_ip_str);

		uint16_t remote_port = 0;
		if (addr->sa_family == AF_INET)
			remote_port = ntohs(((sockaddr_in*)addr)->sin_port);
		if (addr->sa_family == AF_INET6)
			remote_port = ntohs(((sockaddr_in6*)addr)->sin6_port);

		slog_d("SocketUtil::accept tran_socket=%0, remote_ip=%1, remote_port=%2", *svr_trans_socket, remote_ip_str, remote_port);
	}

	return true;
}

bool SocketUtil::connect(socket_t client_socket, const std::string& svr_ip_or_name, int svr_port)
{
	Ip svr_ip = __resovleIpByName(svr_ip_or_name);
	if (svr_ip.m_type == EIpType_none)
	{
		slog_d("SocketUtil::connect fail __getIpByNameOrIpStr");
		return false;
	}

	if (!connect(client_socket, svr_ip, svr_port))
		return false;
	return true;
}

bool SocketUtil::connect(socket_t client_socket, Ip svr_ip, int svr_port)
{
	sockaddr_storage addr;
	int addr_len = 0;
	__initAddrStorage(&addr, &addr_len, svr_ip, svr_port);

	if (::connect(client_socket, (sockaddr*)&addr, addr_len) < 0)
	{
		int err_code = getErr();
#ifdef S_OS_WIN
		if (err_code == WSAEWOULDBLOCK)
#else
		if (err_code == EINPROGRESS)
#endif // S_OS_WIN
			return true;


		slog_d("SocketUtil::connect fail to connect, ip=%0, port=%1!", ipToStr(svr_ip).c_str(), svr_port);
		return false;
	}
	return true;
}

void SocketUtil::disconnect(socket_t client_socket)
{
#ifdef S_OS_WIN
	shutdown(client_socket, SD_BOTH);
#else
	shutdown(client_socket, SHUT_RDWR);
#endif // S_OS_WIN
}

bool SocketUtil::send(socket_t s, const byte_t* data, size_t data_len)
{
	while (true)
	{
		size_t real_send = 0;
		bool isOk = send(s, data, data_len, &real_send);
		if (!isOk)
			return false;
		if (real_send >= data_len) {
			return true;
		}

		data += real_send;
		data_len -= real_send;
	}
	return true;
}

bool SocketUtil::send(socket_t s, const byte_t* data, size_t data_len, size_t* real_send_len)
{
#if defined(S_OS_WIN)
	int ret = ::send(s, (const char*)data, (int)data_len, 0);

	if (ret <= 0)
	{
		*real_send_len = 0;
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}
	else
	{
		*real_send_len = ret;
		return true;
	}
#else
	ssize_t ret = ::send(s, (const char*)data, (int)data_len, 0);

	if (ret <= 0)
	{
		*real_send_len = 0;
		if (errno == EAGAIN)
			return true;
		else
			return false;
	}
	else
	{
		*real_send_len = ret;
		return true;
	}
#endif
}

bool SocketUtil::recv(socket_t s, byte_t* buf, size_t buf_len, size_t* real_recv_len)
{
#if defined(S_OS_WIN)
	*real_recv_len = 0;
	int ret = ::recv(s, (char*)buf, (int)buf_len, 0);

	if (ret < 0)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}
	else if (ret == 0)
	{
		return false;
	}
	else
	{
		*real_recv_len = ret;
		return true;
	}
#else
	*real_recv_len = 0;
	ssize_t ret = ::recv(s, (char*)buf, (int)buf_len, 0);

	if (ret < 0)
	{
		if (ret == EAGAIN)
			return true;
		else
			return false;
	}
	else if (ret == 0)
	{
		return false;
	}
	else
	{
		*real_recv_len = ret;
		return true;
	}
#endif
}

bool SocketUtil::sendTo(socket_t s, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port)
{
	while (true)
	{
		size_t real_send_len = 0;
		bool isOk = sendTo(s, data, data_len, to_ip, to_port, &real_send_len);
		if (!isOk)
			return false;
		if (real_send_len >= data_len) {
			return true;
		}

		data += real_send_len;
		data_len -= real_send_len;
	}
	return true;
}

bool SocketUtil::sendTo(socket_t s, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port, size_t* real_send_len) 
{
	sockaddr_storage addr;
	int addr_len = 0;
	__initAddrStorage(&addr, &addr_len, to_ip, to_port);

#if defined(S_OS_WIN)
	int ret = ::sendto(s, (const char*)data, (int)data_len, 0, (sockaddr*)&addr, addr_len);
	if (ret <= 0)
	{
		*real_send_len = 0;
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}
	else
	{
		*real_send_len = ret;
		return true;
	}
#else
	ssize_t ret = ::sendto(s, (const char*)data, (int)data_len, 0, (sockaddr*)&addr, addr_len);

	if (ret <= 0)
	{
		*real_send_len = 0;
		if (errno == EAGAIN)
			return true;
		else
			return false;
	}
	else
	{
		*real_send_len = ret;
		return true;
	}
#endif
}

bool SocketUtil::recvFrom(socket_t s, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len)
{
	sockaddr_storage addr;
	uint32_t addr_len = sizeof(addr);
	if (!__recvFrom(s, buf, buf_len, &addr, &addr_len, real_recv_len))
		return false;
	if (!SocketUtil::sockaddrToIpAndPort((sockaddr*)&addr, addr_len, from_ip, from_port))
		return false;
	return true;
}

void SocketUtil::closeSocket(socket_t s)
{
#if defined(S_OS_WIN)
	::closesocket(s);
#else
	::close(s);
#endif
}

bool SocketUtil::changeSocketToAsync(socket_t s)
{
#if defined(S_OS_WIN)
	DWORD ul = 1;
	if (0 != ioctlsocket(s, FIONBIO, &ul))
	{
		slog_d("SocketUtil::changeSocketToAsync fail to ioctlsocket");
		return false;
	}

	return true;
#else

	int flags = fcntl(s, F_GETFL, 0);
	if (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		slog_e("SocketUtil::changeSocketToAsync fail to fcntl");
		return false;
	}
	return true;
#endif
}

int SocketUtil::getErr()
{
#if defined(S_OS_WIN)
	return WSAGetLastError();
#else
	return errno;
#endif
}

bool SocketUtil::resolveIpsByName(const std::string& ip_or_name, std::vector<Ip>* ips)
{
	if (ips == nullptr)
		return false;
	ips->clear();

	if (ip_or_name.size() == 0)
		return false;

	Ip ip;
	if (strToIp(ip_or_name, &ip))
	{
		ips->push_back(ip);
		return true;
	}

	addrinfo addr_hint;
	memset(&addr_hint, 0, sizeof(addr_hint));
	addr_hint.ai_family = AF_INET;
	addr_hint.ai_socktype = SOCK_STREAM;

	addrinfo* addr_result = NULL;
	if (getaddrinfo(ip_or_name.c_str(), NULL, &addr_hint, &addr_result) != 0)
	{
		slog_d("SocketUtil::getIpByName fail to getaddrinfo %0", ip_or_name);
		return false;
	}

	char ip_str[100];
	for (addrinfo* a = addr_result; a != NULL; a = a->ai_next)
	{
		if (a->ai_family == AF_INET)
		{
			in_addr ip_v4 = ((sockaddr_in*)a->ai_addr)->sin_addr;
			ips->push_back(Ip(ip_v4));

			if (inet_ntop(AF_INET, &ip_v4, ip_str, sizeof(ip_str)) != nullptr)
				slog_d("%0 ip=%1", ip_or_name, ip_str);
		}
		else if (a->ai_family == AF_INET6)
		{
			in6_addr ip_v6 = ((sockaddr_in6*)a->ai_addr)->sin6_addr;
			ips->push_back(Ip(ip_v6));

			if (inet_ntop(AF_INET6, &ip_v6, ip_str, sizeof(ip_str)) != nullptr)
				slog_d("%0 ip=%1", ip_or_name, ip_str);
		}
	}

	return true;
}

Ip SocketUtil::resolveIpByName(const std::string & ip_or_name)
{
	return __resovleIpByName(ip_or_name);
}





bool SocketUtil::ipToStr(const sockaddr * addr, std::string * ip_str)
{
	if (addr->sa_family == AF_INET)
	{
		return ipv4ToStr(((sockaddr_in*)addr)->sin_addr, ip_str);
	}
	else if (addr->sa_family == AF_INET6)
	{
		return ipv6ToStr(((sockaddr_in6*)addr)->sin6_addr, ip_str);
	}
	else
	{
		return false;
	}
}

bool SocketUtil::ipToStr(Ip ip, std::string * ip_str)
{
	if (ip.m_type == EIpType_v4)
		return ipv4ToStr(ip.m_value.m_v4, ip_str);
	else
		return ipv6ToStr(ip.m_value.m_v6, ip_str);
}

std::string SocketUtil::ipToStr(Ip ip)
{
	std::string str;
	ipToStr(ip, &str);
	return str;
}

bool SocketUtil::ipv4ToStr(in_addr ip_v4, std::string* ip_str)
{
	if (ip_str == nullptr)
		return false;
	*ip_str = "";

	char buf[100];
	if (inet_ntop(AF_INET, &ip_v4, buf, 100) == NULL)
		return false;

	*ip_str = buf;
	return true;
}

bool SocketUtil::ipv6ToStr(in6_addr ip_v6, std::string * ip_str)
{
	if (ip_str == nullptr)
		return false;
	*ip_str = "";

	char buf[100];
	if (inet_ntop(AF_INET6, &ip_v6, buf, 100) == NULL)
		return false;

	*ip_str = buf;
	return true;
}

bool SocketUtil::strToIp(const std::string & ip_str, Ip * ip)
{ 
	if (strToIpV4(ip_str, &ip->m_value.m_v4))
	{
		ip->m_type = EIpType_v4;
		return true;
	}

	if (strToIpV6(ip_str, &ip->m_value.m_v6))
	{
		ip->m_type = EIpType_v6;
		return true;
	}

	return false;
}

Ip SocketUtil::strToIp(const std::string & ip_str)
{
	Ip ip;
	strToIp(ip_str, &ip);
	return ip;
}

bool SocketUtil::strToIpV4(const std::string& ip_str, in_addr* ip_v4)
{
	return inet_pton(AF_INET, ip_str.c_str(), ip_v4) == 1;
}

bool SocketUtil::strToIpV6(const std::string & ip_str, in6_addr * ip_v6)
{
	return inet_pton(AF_INET6, ip_str.c_str(), ip_v6) == 1;
}

bool SocketUtil::sockaddrToIpAndPort(const sockaddr * addr, uint32_t addr_len, Ip* ip, uint32_t * port)
{
	if (addr_len == sizeof(sockaddr_in))
	{
		sockaddr_in* addr_v4 = (sockaddr_in*)addr;
		ip->m_type = EIpType_v4;
		ip->m_value.m_v4 = addr_v4->sin_addr;
		*port = nToHs(addr_v4->sin_port);
	}
	else if (addr_len == sizeof(sockaddr_in6))
	{
		sockaddr_in6* addr_v6 = (sockaddr_in6*)addr;
		ip->m_type = EIpType_v6;
		ip->m_value.m_v6 = addr_v6->sin6_addr;
		*port = nToHs(addr_v6->sin6_port);
	}
	else
	{
		return false;
	}

	return true; 
}

bool SocketUtil::ipAndPortToSockaddr(Ip ip, uint32_t port, sockaddr_storage * addr, uint32_t * addr_len)
{
	if (ip.m_type == EIpType_none)
		return false;
	__initAddrStorage(addr, (int*)addr_len, ip, port);
	return true;
}

bool SocketUtil::isValidSid(socket_id_t s)
{
	return s > 0;
}

uint16_t SocketUtil::hToNs(uint16_t s)
{
	return htons(s);
}

uint32_t SocketUtil::hToNl(uint32_t l)
{
	return htonl(l);
}

uint16_t SocketUtil::nToHs(uint16_t s)
{
	return ntohs(s);
}

uint32_t SocketUtil::nToHl(uint32_t l)
{
	return ntohl(l);
}






void SocketUtil::__initAddrV4(struct sockaddr_in* addr, in_addr ip, int port)
{
	memset(addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr = ip;
	addr->sin_port = htons((short)port);
}

void SocketUtil::__initAddrV6(sockaddr_in6 * addr, in6_addr ip, int port)
{
	memset(addr, 0, sizeof(sockaddr_in6));
	addr->sin6_family = AF_INET;
	addr->sin6_addr = ip;
	addr->sin6_port = htons((short)port);
}

void SocketUtil::__initAddrStorage(sockaddr_storage * addr, int* addr_len, Ip ip, int port)
{
	if (ip.m_type == EIpType_v4)
	{
		SocketUtil::__initAddrV4((sockaddr_in*)addr, ip.m_value.m_v4, port);
		*addr_len = sizeof(sockaddr_in);
	}
	else
	{
		SocketUtil::__initAddrV6((sockaddr_in6*)addr, ip.m_value.m_v6, port);
		*addr_len = sizeof(sockaddr_in6);
	}
}

Ip SocketUtil::__resovleIpByName(const std::string& ip_or_name)
{
	Ip ip;
	std::vector<Ip> ips;
	if (!SocketUtil::resolveIpsByName(ip_or_name.c_str(), &ips))
	{
		return ip;
	}
	if (ips.size() == 0 || ips[0].m_type == EIpType_none)
	{
		return ip;
	}

	ip = ips[0];
	return ip;
}

bool SocketUtil::__recvFrom(socket_t s, byte_t* buf, size_t buf_len, sockaddr_storage* addr, uint32_t* addr_len, size_t* real_recv_len)
{
	*real_recv_len = 0;
#if defined(S_OS_WIN)
	int ret = ::recvfrom(s, (char*)buf, (int)buf_len, 0, (sockaddr*)addr, (int*)addr_len);
	if (ret < 0)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;
		else
			return false;
	}
	else if (ret == 0)
	{
		return false;
	}
	else
	{
		*real_recv_len = ret;
		return true;
	}
#else
	ssize_t ret = ::recvfrom(s, (char*)buf, (int)buf_len, 0, (sockaddr*)addr, (socklen_t*)addr_len);

	if (ret < 0)
	{
		if (ret == EAGAIN)
			return true;
		else
			return false;
	}
	else if (ret == 0)
	{
		return false;
	}
	else
	{
		*real_recv_len = ret;
		return true;
	}
#endif
}


S_NAMESPACE_END

