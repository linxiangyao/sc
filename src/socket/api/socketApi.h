#ifndef S_TCP_SOCKET_CALLBACK_API_H_
#define S_TCP_SOCKET_CALLBACK_API_H_
#include "../base/iSocketApi.h"

#include "../dns/dnsResolver.h"

#include "../tcp/tcpAsyncClientApi.h"
#include "../tcp/tcpAsyncServerApi.h"
#include "../tcp/tcpSyncApi.h"

#include "../udp/udpAsyncApi.h"
#include "../udp/udpSyncApi.h"

#include "../util/socketUtil.h"
S_NAMESPACE_BEGIN




/*
socketApi typical is a singleton.
*/
class SocketApi : public ISocketApi, private IMessageLoopHandler
{
public:
	SocketApi();
	~SocketApi();
	virtual bool init(MessageLooper* work_looper = nullptr);

	virtual bool tcp_open(socket_id_t * s) override;
	virtual void tcp_close(socket_id_t s) override;
	virtual bool tcp_bindAndListen(socket_id_t svr_socket, const std::string & svr_ip_or_name, int svr_port) override;
	virtual bool tcp_accept(socket_id_t svr_socket, socket_id_t * svr_tran_socket) override;
	virtual bool tcp_connect(socket_id_t client_socket, const std::string & svr_ip_or_name, int svr_port) override;
	virtual void tcp_disconnect(socket_id_t client_socket) override;
	virtual bool tcp_send(socket_id_t s, const byte_t * data, size_t data_len) override;
	virtual bool tcp_recv(socket_id_t s, byte_t * buf, size_t buf_len, size_t * recv_len) override;

	virtual bool atcp_createClientSocket(socket_id_t * client_sid, const TcpClientSocketCreateParam & param) override;
	virtual void atcp_releaseClientSocket(socket_id_t client_sid) override;
	virtual bool atcp_startClientSocket(socket_id_t client_sid, uint64_t* connection_id) override;
	virtual void atcp_stopClientSocket(socket_id_t client_sid, uint64_t connection_id) override;
	virtual bool atcp_sendDataFromClientSocketToSvr(socket_id_t client_sid, uint64_t connection_id, const byte_t * data, size_t data_len) override;
	virtual bool atcp_createSvrListenSocket(socket_id_t * svr_listen_sid, const TcpSvrListenSocketCreateParam & param) override;
	virtual void atcp_releaseSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual bool atcp_startSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual void atcp_stopSvrListenSocket(socket_id_t svr_listen_sid) override;
	virtual void atcp_stopSvrTranSocket(socket_id_t svr_tran_sid) override;
	virtual bool atcp_sendDataFromSvrTranSocketToClient(socket_id_t svr_tran_sid, const byte_t * data, size_t data_len) override;

	virtual bool udp_open(socket_id_t* sid) override;
	virtual void udp_close(socket_id_t sid) override;
	virtual bool udp_sendTo(socket_id_t sid, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port) override;
	virtual bool udp_recvFrom(socket_id_t sid, byte_t* buf, size_t buf_len, Ip* from_ip, uint32_t* from_port, size_t* real_recv_len) override;

	virtual bool audp_createSocket(socket_id_t * sid, const UdpSocketCreateParam & param) override;
	virtual void audp_releaseSocket(socket_id_t sid) override;
	virtual bool audp_sendTo(socket_id_t sid, uint64_t send_id, const byte_t * data, size_t data_len, Ip to_ip, uint32_t to_port) override;

	virtual void stopDnsResolver() override;
	virtual bool addDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target) override;
	virtual void removeDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target) override;
	virtual bool getDnsRecordByName(const std::string & name, DnsRecord * record) override;
	virtual bool startToResolveDns(const std::string & name) override;






private:
	virtual void onMessage(Message * msg, bool * is_handled) override;
	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data) override;

	MessageLooper* m_work_looper;
	MessageLoopThread* m_work_thread;

	DnsResolver* m_dns_resolver;
	TcpSyncApi* m_tcp_sync_api;
	TcpAsyncClientApi* m_tcp_async_client_api;
	TcpAsyncServerApi* m_tcp_async_server_api;
	UdpAsyncApi* m_udp_async_api;
	UdpSyncApi* m_udp_sync_api;
};






S_NAMESPACE_END
#endif


