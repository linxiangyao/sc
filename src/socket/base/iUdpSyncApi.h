#ifndef S_IUDP_SYNC_API_H_
#define S_IUDP_SYNC_API_H_
#include "socketType.h"
S_NAMESPACE_BEGIN



class IUdpSyncApi
{
public:
	virtual ~IUdpSyncApi() {}
	virtual bool udp_open(socket_id_t* sid) = 0;
	virtual void udp_close(socket_id_t sid) = 0;
	virtual bool udp_bind(socket_id_t sid, Ip ip, uint32_t port) = 0;
	virtual bool udp_sendTo(socket_id_t sid, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port) = 0;
	virtual bool udp_recvFrom(socket_id_t sid, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len) = 0;

};


S_NAMESPACE_END
#endif


