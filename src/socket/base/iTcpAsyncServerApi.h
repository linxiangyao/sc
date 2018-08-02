#ifndef S_ITCP_ASYNC_SERVER_API_H_
#define S_ITCP_ASYNC_SERVER_API_H_
#include "socketType.h"
S_NAMESPACE_BEGIN


/*
tcp async server api.
*/
class ITcpAsyncServerApi
{
public:
	enum EMsgType
	{
		EMsgType_tcpSvrListenSocketStarted = -67125615,
		EMsgType_tcpSvrListenSocketStopped,
		EMsgType_tcpSvrListenSocketAccepted,

		EMsgType_tcpSvrTranSocketRecvData,
		EMsgType_tcpSvrTranSocketSendDataEnd,
		EMsgType_tcpSvrTranSocketStopped,
	};

	class TcpSvrListenSocketCreateParam
	{
	public:
		TcpSvrListenSocketCreateParam() { m_notify_looper = NULL; m_notify_target = NULL; m_svr_port = 0; }

		MessageLooper* m_notify_looper;
		void* m_notify_target;
		std::string m_svr_ip;
		uint32_t m_svr_port;
	};

	class Msg_TcpSvrListenSocketStarted : public Message
	{
	public:
		Msg_TcpSvrListenSocketStarted() { m_msg_type = EMsgType_tcpSvrListenSocketStarted; m_svr_listen_sid = 0; }
		socket_id_t m_svr_listen_sid;
	};

	class Msg_TcpSvrListenSocketStopped : public Message
	{
	public:
		Msg_TcpSvrListenSocketStopped() { m_msg_type = EMsgType_tcpSvrListenSocketStopped; m_svr_listen_sid = 0; }
		socket_id_t m_svr_listen_sid;
	};

	class Msg_TcpSvrListenSocketAccepted : public Message
	{
	public:
		Msg_TcpSvrListenSocketAccepted() { m_msg_type = EMsgType_tcpSvrListenSocketAccepted; m_svr_listen_sid = 0; m_svr_trans_sid = 0; }
		socket_id_t m_svr_listen_sid;
		socket_id_t m_svr_trans_sid;
	};

	class Msg_TcpSvrTranSocketRecvData : public Message
	{
	public:
		Msg_TcpSvrTranSocketRecvData() { m_msg_type = EMsgType_tcpSvrTranSocketRecvData; m_svr_listen_sid = 0; m_svr_trans_sid = 0; }
		socket_id_t m_svr_listen_sid;
		socket_id_t m_svr_trans_sid;
		Binary m_data;
	};

	class Msg_TcpSvrTranSocketSendDataEnd : public Message
	{
	public:
		Msg_TcpSvrTranSocketSendDataEnd() { m_msg_type = EMsgType_tcpSvrTranSocketSendDataEnd; m_svr_listen_sid = 0; m_svr_trans_sid = 0; }
		socket_id_t m_svr_listen_sid;
		socket_id_t m_svr_trans_sid;
	};

	class Msg_TcpSvrTranSocketStopped : public Message
	{
	public:
		Msg_TcpSvrTranSocketStopped() { m_msg_type = EMsgType_tcpSvrTranSocketStopped; m_svr_listen_sid = 0; m_svr_trans_sid = 0; }
		socket_id_t m_svr_listen_sid;
		socket_id_t m_svr_trans_sid;
	};

	virtual ~ITcpAsyncServerApi() {}



	// svr listen
	virtual bool atcp_createSvrListenSocket(socket_id_t* svr_listen_sid, const TcpSvrListenSocketCreateParam& param) = 0;
	virtual void atcp_releaseSvrListenSocket(socket_id_t svr_listen_sid) = 0;
	virtual bool atcp_startSvrListenSocket(socket_id_t svr_listen_sid) = 0;
	virtual void atcp_stopSvrListenSocket(socket_id_t svr_listen_sid) = 0;

	// svr tran
	virtual void atcp_stopSvrTranSocket(socket_id_t svr_tran_sid) = 0;
	virtual bool atcp_sendDataFromSvrTranSocketToClient(socket_id_t svr_tran_sid, const byte_t* data, size_t data_len) = 0;
};



S_NAMESPACE_END
#endif


