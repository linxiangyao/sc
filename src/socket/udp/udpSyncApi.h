#ifndef S_UDP_SYNC_API_H_
#define S_UDP_SYNC_API_H_
#include "../base/iSocketApi.h"
S_NAMESPACE_BEGIN


class UdpSyncApi : public IUdpSyncApi
{
public:
	virtual bool udp_open(socket_id_t * sid) override;
	virtual void udp_close(socket_id_t sid) override;
	virtual bool udp_bind(socket_id_t sid, Ip ip, uint32_t port) override;
	virtual bool udp_sendTo(socket_id_t sid, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port) override;
	virtual bool udp_recvFrom(socket_id_t sid, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len) override;


private:
	bool __getSocketBySid(socket_id_t sid, socket_t* s);
	typedef std::map<socket_id_t, socket_t> SidToSocketMap;
	SidToSocketMap m_sid_2_socket;
	Mutex m_mutex;
};





S_NAMESPACE_END
#endif


