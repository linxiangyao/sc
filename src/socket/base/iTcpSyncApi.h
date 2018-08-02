#ifndef S_ITCP_SYNC_API_H_
#define S_ITCP_SYNC_API_H_
#include "socketType.h"
S_NAMESPACE_BEGIN




/*
tcp sync api.
*/
class ITcpSyncApi
{
public:
	virtual ~ITcpSyncApi() {}

	// create/close
	virtual bool tcp_open(socket_id_t* sid) = 0;
	virtual void tcp_close(socket_id_t sid) = 0;

	// svr
	virtual bool tcp_bindAndListen(socket_id_t svr_listen_sid, const std::string& svr_ip_or_name, int svr_port) = 0;
	virtual bool tcp_accept(socket_id_t svr_listen_sid, socket_id_t* svr_tran_socket) = 0;

	// client
	virtual bool tcp_connect(socket_id_t client_sid, const std::string& svr_ip_or_name, int svr_port) = 0;
	virtual void tcp_disconnect(socket_id_t client_sid) = 0;

	// transport
	virtual bool tcp_send(socket_id_t sid, const byte_t* data, size_t data_len) = 0;
	virtual bool tcp_recv(socket_id_t sid, byte_t* buf, size_t buf_len, size_t* recv_len) = 0;
};



S_NAMESPACE_END
#endif


