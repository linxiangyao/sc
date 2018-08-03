#include "socketApi.h"
S_NAMESPACE_BEGIN

#if defined(S_OS_WIN)
bool initSocketLib()
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return false;
	}
	return true;
}

void releaseSocketLib()
{
	WSACleanup();
}
#else
bool initSocketLib()
{
	return true;
}

void releaseSocketLib()
{
}
#endif



SocketApi::SocketApi()
{
	slog_d("SocketApi:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
	m_work_thread = nullptr;

	m_dns_resolver = nullptr;
	m_tcp_sync_api = nullptr;
	m_tcp_async_client_api = nullptr;
	m_tcp_async_server_api = nullptr;
	m_udp_async_api = nullptr;
	m_udp_sync_api = nullptr;
}

SocketApi::~SocketApi()
{
	slog_d("SocketApi delete=%0", (uint64_t)this);
	delete m_dns_resolver;
	delete m_tcp_sync_api;
	delete m_tcp_async_client_api;
	delete m_tcp_async_server_api;
	delete m_udp_async_api;
	delete m_udp_sync_api;

	if (m_work_thread != NULL)
	{
		m_work_thread->stopAndJoin();
		delete m_work_thread;
	}
	else
	{
		m_work_looper->removeMsgHandler(this);
	}
}

bool SocketApi::init(MessageLooper * work_looper)
{
	slog_d("SocketApi:: init ");
	if (m_work_looper != nullptr)
	{
		slog_w("SocketApi:: init, m_work_looper != nullptr, maybe already init? igonre.");
		return true;
	}

	if (work_looper == nullptr)
	{
		m_work_thread = new MessageLoopThread(this, false);
		m_work_looper = m_work_thread->getLooper();
		if (!m_work_thread->start())
		{
			slog_e("SocketApi:: init fail to start work_thread");
			DELETE_AND_NULL(m_work_thread);
			return false;
		}
	}
	else
	{
		m_work_looper = work_looper;
		m_work_looper->addMsgHandler(this);
	}

	m_dns_resolver = new DnsResolver();
	if (!m_dns_resolver->init(m_work_looper))
		return false;

	m_tcp_sync_api = new TcpSyncApi();
	
	m_tcp_async_client_api = new TcpAsyncClientApi();
	if (!m_tcp_async_client_api->init(m_work_looper, this))
		return false;

	m_tcp_async_server_api = new TcpAsyncServerApi();
	if (!m_tcp_async_server_api->init(m_work_looper, this))
		return false;

	m_udp_sync_api = new UdpSyncApi();

	m_udp_async_api = new UdpAsyncApi();
	if (!m_udp_async_api->init(m_work_looper, this))
		return false;

	return true;
}

bool SocketApi::tcp_open(socket_id_t * s)
{
	return m_tcp_sync_api->tcp_open(s);
}

void SocketApi::tcp_close(socket_id_t s)
{
	m_tcp_sync_api->tcp_close(s);
}

bool SocketApi::tcp_bindAndListen(socket_id_t svr_socket, const std::string & svr_ip_or_name, int svr_port)
{
	return m_tcp_sync_api->tcp_bindAndListen(svr_socket, svr_ip_or_name, svr_port);
}

bool SocketApi::tcp_accept(socket_id_t svr_socket, socket_id_t * svr_tran_socket)
{
	return m_tcp_sync_api->tcp_accept(svr_socket, svr_tran_socket);
}

bool SocketApi::tcp_connect(socket_id_t client_socket, const std::string & svr_ip_or_name, int svr_port)
{
	return m_tcp_sync_api->tcp_connect(client_socket, svr_ip_or_name, svr_port);
}

void SocketApi::tcp_disconnect(socket_id_t client_socket)
{
	return m_tcp_sync_api->tcp_disconnect(client_socket);
}

bool SocketApi::tcp_send(socket_id_t s, const byte_t * data, size_t data_len)
{
	return m_tcp_sync_api->tcp_send(s, data, data_len);
}

bool SocketApi::tcp_recv(socket_id_t s, byte_t * buf, size_t buf_len, size_t * recv_len)
{
	return m_tcp_sync_api->tcp_recv(s, buf, buf_len, recv_len);
}

bool SocketApi::atcp_createClientSocket(socket_id_t * client_sid, const TcpClientSocketCreateParam & param)
{
	return m_tcp_async_client_api->atcp_createClientSocket(client_sid, param);
}

void SocketApi::atcp_releaseClientSocket(socket_id_t client_sid)
{
	m_tcp_async_client_api->atcp_releaseClientSocket(client_sid);
}

bool SocketApi::atcp_startClientSocket(socket_id_t client_sid, uint64_t* connection_id)
{
	return m_tcp_async_client_api->atcp_startClientSocket(client_sid, connection_id);
}

void SocketApi::atcp_stopClientSocket(socket_id_t client_sid, uint64_t connection_id)
{
	m_tcp_async_client_api->atcp_stopClientSocket(client_sid, connection_id);
}

bool SocketApi::atcp_sendDataFromClientSocketToSvr(socket_id_t client_sid, uint64_t connection_id, const byte_t * data, size_t data_len)
{
	return m_tcp_async_client_api->atcp_sendDataFromClientSocketToSvr(client_sid, connection_id, data, data_len);
}

bool SocketApi::atcp_createSvrListenSocket(socket_id_t * svr_listen_sid, const TcpSvrListenSocketCreateParam & param)
{
	return m_tcp_async_server_api->atcp_createSvrListenSocket(svr_listen_sid, param);
}

void SocketApi::atcp_releaseSvrListenSocket(socket_id_t svr_listen_sid)
{
	return m_tcp_async_server_api->atcp_releaseSvrListenSocket(svr_listen_sid);
}

bool SocketApi::atcp_startSvrListenSocket(socket_id_t svr_listen_sid)
{
	return m_tcp_async_server_api->atcp_startSvrListenSocket(svr_listen_sid);
}

void SocketApi::atcp_stopSvrListenSocket(socket_id_t svr_listen_sid)
{
	m_tcp_async_server_api->atcp_stopSvrListenSocket(svr_listen_sid);
}

void SocketApi::atcp_stopSvrTranSocket(socket_id_t svr_tran_sid)
{
	m_tcp_async_server_api->atcp_stopSvrTranSocket(svr_tran_sid);
}

bool SocketApi::atcp_sendDataFromSvrTranSocketToClient(socket_id_t svr_tran_sid, const byte_t * data, size_t data_len)
{
	return m_tcp_async_server_api->atcp_sendDataFromSvrTranSocketToClient(svr_tran_sid, data, data_len);
}

bool SocketApi::udp_open(socket_id_t* sid) 
{ 
	return m_udp_sync_api->udp_open(sid); 
}

void SocketApi::udp_close(socket_id_t sid)
{
	m_udp_sync_api->udp_close(sid);
}

bool SocketApi::udp_bind(socket_id_t sid, Ip ip, uint32_t port)
{
	return m_udp_sync_api->udp_bind(sid, ip, port);
}

bool SocketApi::udp_sendTo(socket_id_t sid, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port) 
{
	return m_udp_sync_api->udp_sendTo(sid, data, data_len, to_ip, to_port);
}

bool SocketApi::udp_recvFrom(socket_id_t sid, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len) 
{
	return m_udp_sync_api->udp_recvFrom(sid, buf, buf_len, from_ip, from_port, real_recv_len);
}

bool SocketApi::audp_createSocket(socket_id_t * sid, const UdpSocketCreateParam & param)
{
	return m_udp_async_api->audp_createSocket(sid, param);
}

void SocketApi::audp_releaseSocket(socket_id_t sid)
{
	m_udp_async_api->audp_releaseSocket(sid);
}

bool SocketApi::audp_sendTo(socket_id_t sid, uint64_t send_id, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port)
{
	return m_udp_async_api->audp_sendTo(sid, send_id, data, data_len, to_ip, to_port);
}

void SocketApi::stopDnsResolver()
{
	m_dns_resolver->stopDnsResolver();
}

bool SocketApi::addDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target)
{
	return m_dns_resolver->addDnsResolverNotifyLooper(notify_loop, notify_target);
}

void SocketApi::removeDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target)
{
	m_dns_resolver->removeDnsResolverNotifyLooper(notify_loop, notify_target);
}

bool SocketApi::getDnsRecordByName(const std::string & name, DnsRecord * record)
{
	return m_dns_resolver->getDnsRecordByName(name, record);
}

bool SocketApi::startToResolveDns(const std::string & name)
{
	return m_dns_resolver->startToResolveDns(name);
}

void SocketApi::onMessage(Message * msg, bool * is_handled)
{
}

void SocketApi::onMessageTimerTick(uint64_t timer_id, void * user_data)
{
}





S_NAMESPACE_END
