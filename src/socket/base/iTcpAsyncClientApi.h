#ifndef S_ITCP_ASYNC_CLIENT_API_H_
#define S_ITCP_ASYNC_CLIENT_API_H_
#include "socketType.h"
S_NAMESPACE_BEGIN



/*
tcp async client api.
*/
class ITcpAsyncClientApi
{
public:
	enum EMsgType
	{
		EMsgType_tcpClientSocketConnected = -87634851,
		EMsgType_tcpClientSocketDisconnected,
		EMsgType_tcpClientSocketRecvData,
		EMsgType_tcpClientSocketSendDataEnd,
	};

	class TcpClientSocketCreateParam
	{
	public:
		TcpClientSocketCreateParam()  { m_notify_looper = NULL; m_notify_target = NULL; m_svr_port = 0; }

		MessageLooper* m_notify_looper;
		void* m_notify_target;
		Ip m_svr_ip;
		uint32_t m_svr_port;
	};

	class Msg_TcpClientSocketConnected : public Message
	{
	public:
		Msg_TcpClientSocketConnected() { m_msg_type = EMsgType_tcpClientSocketConnected; m_client_sid = 0; m_connection_id = 0; }
		socket_id_t m_client_sid;
		uint64_t m_connection_id;
	};

	class Msg_TcpClientSocketDisconnected : public Message
	{
	public:
		Msg_TcpClientSocketDisconnected() { m_msg_type = EMsgType_tcpClientSocketDisconnected; m_client_sid = 0; m_connection_id = 0; }
		socket_id_t m_client_sid;
		uint64_t m_connection_id;
	};

	class Msg_TcpClientSocketSendDataEnd : public Message
	{
	public:
		Msg_TcpClientSocketSendDataEnd() { m_msg_type = EMsgType_tcpClientSocketSendDataEnd; m_client_sid = 0; m_connection_id = 0; }
		socket_id_t m_client_sid;
		uint64_t m_connection_id;
	};

	class Msg_TcpClientSocketRecvData : public Message
	{
	public:
		Msg_TcpClientSocketRecvData() { m_msg_type = EMsgType_tcpClientSocketRecvData; m_client_sid = 0; m_connection_id = 0; }
		socket_id_t m_client_sid;
		uint64_t m_connection_id;
		Binary m_recv_data;
	};



	virtual ~ITcpAsyncClientApi() {}

	virtual bool atcp_createClientSocket(socket_id_t* client_sid, const TcpClientSocketCreateParam& param) = 0;
	virtual void atcp_releaseClientSocket(socket_id_t client_sid) = 0;
	virtual bool atcp_startClientSocket(socket_id_t client_sid, uint64_t* connection_id) = 0;
	virtual void atcp_stopClientSocket(socket_id_t client_sid, uint64_t connection_id) = 0;
	virtual bool atcp_sendDataFromClientSocketToSvr(socket_id_t client_sid, uint64_t connection_id, const byte_t* data, size_t data_len) = 0;
};





S_NAMESPACE_END
#endif

