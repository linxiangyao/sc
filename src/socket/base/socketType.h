#ifndef S_SOCKET_TYPE_H_
#define S_SOCKET_TYPE_H_
#include <vector>
#include <stdio.h>
#include "../../comm/comm.h"
#include "../../util/stringUtil.h"
#include "../../thread/threadLib.h"

#if defined(S_OS_WIN)
#pragma comment(lib,"ws2_32")
#else
	#include <vector>
	#include <stdio.h>
	#include <errno.h>
	#include <string.h>
	#include <stdlib.h>

	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <sys/shm.h>
#endif
S_NAMESPACE_BEGIN






#ifdef WIN32
#else
	#define SOCKET int
	#define INVALID_SOCKET -1
#endif


typedef SOCKET socket_t;
typedef int64_t socket_id_t;


enum ETcpSocketType
{
    ETcpSocketType_client,
    ETcpSocketType_svr_listen,
    ETcpSocketType_svr_tran,
};

enum ESocketType
{
	ESocketType_tcp,
	ESocketType_udp,
};


enum EIpType
{
	EIpType_none,
	EIpType_v4,
	EIpType_v6,
};



class Ip
{
public:
	Ip() { m_type = EIpType_none; memset(&m_value, 0, sizeof(m_value)); }
	Ip(in_addr v4) { m_type = EIpType_v4; __cpV4(&m_value.m_v4, &v4); }
	Ip(in6_addr v6) { m_type = EIpType_v6; __cpV6(&m_value.m_v6, &v6); }
	Ip(const Ip& ip) { __cpIp(ip); }
	Ip& operator =(const Ip& ip) { __cpIp(ip); return *this; }
	

	EIpType m_type;
	union
	{
		in_addr m_v4;
		in6_addr m_v6;
	} m_value;

private:
	void __cpIp(const Ip& ip)
	{
		m_type = ip.m_type;
		__cpV6(&m_value.m_v6, &ip.m_value.m_v6);
	}

	static void __cpV4(in_addr* to, const in_addr* from)
	{
		memcpy(to, from, sizeof(in_addr));
	}

	static void __cpV6(in6_addr* to, const in6_addr* from)
	{
		memcpy(to, from, sizeof(in6_addr));
	}
};







S_NAMESPACE_END
#endif //S_SOCKET_API_TYPE_H_


