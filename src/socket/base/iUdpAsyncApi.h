#ifndef S_IUDP_ASYNC_API_H_
#define S_IUDP_ASYNC_API_H_
#include "socketType.h"
S_NAMESPACE_BEGIN






class IUdpAsyncApi
{
public:
	enum EMsgType
	{
		EMsgType_udpSocketClosed = -267427361,
		EMsgType_udpSocketRecvData,
		EMsgType_udpSocketSendDataEnd,
	};

	class UdpSocketCreateParam
	{
	public:
		UdpSocketCreateParam() { m_notify_looper = NULL; m_notify_target = NULL; m_is_bind = false; m_bind_port = 0;}

		MessageLooper* m_notify_looper;
		void* m_notify_target;
		Ip m_bind_ip;
		uint32_t m_bind_port;
		bool m_is_bind;
	};

	class Msg_UdpSocketClosed : public Message
	{
	public:
		Msg_UdpSocketClosed() { m_msg_type = EMsgType_udpSocketClosed; m_sid = 0; }
		socket_id_t m_sid;
	};

	class Msg_UdpSocketSendDataEnd : public Message
	{
	public:
		Msg_UdpSocketSendDataEnd() { m_msg_type = EMsgType_udpSocketSendDataEnd; m_sid = 0; }
		socket_id_t m_sid;
		//Ip m_to_ip;
		//uint32_t m_to_port;
	};

	class Msg_UdpSocketRecvData : public Message
	{
	public:
		Msg_UdpSocketRecvData() { m_msg_type = EMsgType_udpSocketRecvData; m_sid = 0; m_from_port = 0; }
		socket_id_t m_sid;
		Binary m_recv_data;
		Ip m_from_ip;
		uint32_t m_from_port;
	};

	virtual ~IUdpAsyncApi() {}



	virtual bool audp_createSocket(socket_id_t* sid, const UdpSocketCreateParam& param) = 0;
	virtual void audp_releaseSocket(socket_id_t sid) = 0;
	virtual bool audp_sendTo(socket_id_t sid, uint64_t send_id, const byte_t* data, size_t data_len, Ip to_ip, uint32_t to_port) = 0;
};




S_NAMESPACE_END
#endif


