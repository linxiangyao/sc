#ifndef S_TCP_SYNC_API_H_
#define S_TCP_SYNC_API_H_
#include "../base/iSocketApi.h"
S_NAMESPACE_BEGIN




class TcpSyncApi : public ITcpSyncApi
{
public:
	TcpSyncApi();
	~TcpSyncApi();

	// create/close
	bool tcp_open(socket_id_t* sid);
	void tcp_close(socket_id_t sid);

	// svr
	bool tcp_bindAndListen(socket_id_t svr_listen_sid, const std::string& svr_ip_or_name, int svr_port);
	bool tcp_accept(socket_id_t svr_listen_sid, socket_id_t* svr_tran_sid);

	// client
	bool tcp_connect(socket_id_t client_sid, const std::string& svr_ip_or_name, int svr_port);
	void tcp_disconnect(socket_id_t client_sid);

	// transport
	bool tcp_send(socket_id_t sid, const byte_t* data, size_t data_len);
	bool tcp_recv(socket_id_t sid, byte_t* buf, size_t buf_len, size_t* recv_len);




private:
	bool __getSocketBySid(socket_id_t sid, socket_t* socket);

	typedef std::map<socket_id_t, socket_t> SidToSocketMap;
	SidToSocketMap m_sid_2_socket;
	Mutex m_mutex;
};





S_NAMESPACE_END
#endif


