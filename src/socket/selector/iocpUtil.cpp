#include "iocpUtil.h"
#include "../util/socketUtil.h"
S_NAMESPACE_BEGIN
#ifdef S_OS_WIN



IocpUtil::MyOverlap * IocpUtil::newMyOverlap(socket_t socket, uint64_t session_id, EIocpCmdType cmd_type, uint64_t cmd_id)
{
	MyOverlap* overlap = new MyOverlap();
	overlap->m_socket = socket;
	overlap->m_session_id = session_id;
	overlap->m_cmd_type = cmd_type;
	overlap->m_cmd_id = cmd_id;
	overlap->m_buffer.buf = NULL;
	overlap->m_buffer.len = 0;
	return overlap;
}

void IocpUtil::deleteMyOverlap(MyOverlap * overlap)
{
	delete[] overlap->m_buffer.buf;
	delete overlap;
}

HANDLE IocpUtil::createCompletionPort(DWORD dwNumberOfConcurrentThreads) {
	return(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads));
}

BOOL IocpUtil::associateDeviceWithCompletionPort(HANDLE hCompletionPort, HANDLE hDevice, DWORD dwCompletionKey) {
	HANDLE h = CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);
	return h == hCompletionPort;
}

uint64_t IocpUtil::genCmdId()
{
	static Mutex m;
	static uint64_t s_id_seed = 0;
	ScopeMutex __l(m);
	return ++s_id_seed;
}

bool IocpUtil::postRecvCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id)
{
	const int __OVERLAP_RECV_BUF_LEN = (1024 * 10);
	MyOverlap* overlap = newMyOverlap(socket, session_id, EIocpCmdType_recv, cmd_id);
	overlap->m_buffer.buf = new char[__OVERLAP_RECV_BUF_LEN];
	overlap->m_buffer.len = __OVERLAP_RECV_BUF_LEN;
	DWORD flag = 0;
	DWORD numberOfBytesRecvd = 0;
	int ret = WSARecv(socket, &overlap->m_buffer, 1, &numberOfBytesRecvd, &flag,
		&overlap->m_overlap, NULL);
	if (ret != 0)
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			slog_d("IocpUtil:: postRecvCmd WSA_IO_PENDING, id=%0", overlap->m_cmd_id);
			return true;
		}
		else
		{
			slog_d("IocpUtil:: postRecvCmd fail to WSARecv");
			deleteMyOverlap(overlap);
			return false;
		}
	}
	else
	{
		slog_d("IocpUtil:: postRecvCmd already recv, id=%0, len=%1", overlap->m_cmd_id, (uint32_t)numberOfBytesRecvd);
		//byte_t* data = new byte_t[numberOfBytesRecvd + 1];
		//memset(data, 0, numberOfBytesRecvd + 1);
		//memcpy(data, overlap->m_buffer.buf, numberOfBytesRecvd);
		//printf("iocp_svr_tran:: __postRecvCmd already recv, id=%lld, len=%d, data=%s\n", overlap->m_cmd_id, numberOfBytesRecvd, (const char*)data);
	}

	return true;
}

bool IocpUtil::postSendCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id, const byte_t * buf, size_t buf_len)
{
	MyOverlap* overlap = IocpUtil::newMyOverlap(socket, session_id, EIocpCmdType_send, cmd_id);
	overlap->m_buffer.buf = new char[buf_len];
	memcpy(overlap->m_buffer.buf, buf, buf_len);
	overlap->m_buffer.len = (ULONG)buf_len;
	DWORD numOfByteSent = 0;

	int ret = WSASend(socket, &overlap->m_buffer, 1, &numOfByteSent, 0, &overlap->m_overlap, NULL);

	if (ret != 0)
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			slog_d("IocpUtil:: postSendCmd WSA_IO_PENDING, id=%0", overlap->m_cmd_id);
			return true;
		}
		else
		{
			slog_d("IocpUtil:: postSendCmd fail to WSASend");
			IocpUtil::deleteMyOverlap(overlap);
			return false;
		}
	}
	else
	{
		slog_d("IocpUtil:: postSendCmd already sent, id=%0, len=%1", overlap->m_cmd_id, (uint32_t)numOfByteSent);
	}

	return true;
}

bool IocpUtil::postRecvFromCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id)
{
	const int __OVERLAP_RECV_BUF_LEN = (1024 * 10);
	MyOverlap* overlap = newMyOverlap(socket, session_id, EIocpCmdType_recvFrom, cmd_id);
	overlap->m_buffer.buf = new char[__OVERLAP_RECV_BUF_LEN];
	overlap->m_buffer.len = __OVERLAP_RECV_BUF_LEN;
	DWORD flag = 0;
	DWORD numberOfBytesRecvd = 0;

	int ret = WSARecvFrom(socket, &overlap->m_buffer, 1, &numberOfBytesRecvd, &flag, (sockaddr*)&overlap->m_addr, &overlap->m_addr_len,
		&overlap->m_overlap, NULL);

	if (ret != 0)
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			slog_d("IocpUtil:: postRecvFromCmd WSA_IO_PENDING, id=%0", overlap->m_cmd_id);
			return true;
		}
		else
		{
			slog_d("IocpUtil:: postRecvFromCmd fail to WSARecvFrom");
			deleteMyOverlap(overlap);
			return false;
		}
	}
	else
	{
		slog_d("IocpUtil:: postRecvFromCmd already recv, id=%0, len=%1", overlap->m_cmd_id, (uint32_t)numberOfBytesRecvd);
		//byte_t* data = new byte_t[numberOfBytesRecvd + 1];
		//memset(data, 0, numberOfBytesRecvd + 1);
		//memcpy(data, overlap->m_buffer.buf, numberOfBytesRecvd);
		//printf("iocp_svr_tran:: postRecvFromCmd already recv, id=%lld, len=%d, data=%s\n", overlap->m_cmd_id, numberOfBytesRecvd, (const char*)data);
	}

	return true;
}

bool IocpUtil::postSendToCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id, Ip to_ip, uint32_t to_port, const byte_t * buf, size_t buf_len)
{
	MyOverlap* overlap = IocpUtil::newMyOverlap(socket, session_id, EIocpCmdType_sendTo, cmd_id);
	overlap->m_buffer.buf = new char[buf_len];
	memcpy(overlap->m_buffer.buf, buf, buf_len);
	overlap->m_buffer.len = (ULONG)buf_len;
	DWORD numOfByteSent = 0;
	sockaddr_storage addr;
	uint32_t addr_len = 0;
	SocketUtil::ipAndPortToSockaddr(to_ip, to_port, &addr, &addr_len);

	int ret = WSASendTo(socket, &overlap->m_buffer, 1, &numOfByteSent, 0, (sockaddr*)&addr, addr_len,
		&overlap->m_overlap, NULL);

	if (ret != 0)
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			slog_d("IocpUtil:: postSendToCmd WSA_IO_PENDING, id=%0", overlap->m_cmd_id);
			return true;
		}
		else
		{
			slog_d("IocpUtil:: postSendToCmd fail to WSASendTo");
			IocpUtil::deleteMyOverlap(overlap);
			return false;
		}
	}
	else
	{
		slog_d("IocpUtil:: postSendToCmd already sent, id=%0, len=%1", overlap->m_cmd_id, (uint32_t)numOfByteSent);
	}

	return true;
}

void IocpUtil::postExitCmd(HANDLE completionPort)
{
	MyOverlap* overlap = IocpUtil::newMyOverlap(INVALID_SOCKET, -1, EIocpCmdType_exit, genCmdId());
	overlap->m_cmd_type = EIocpCmdType_exit;
	PostQueuedCompletionStatus(completionPort, 0, 0, (LPOVERLAPPED)overlap);
}

void IocpUtil::__copyOverlapToMsg(IocpUtil::MyOverlap* overlap, SocketSelector::BaseMsg* msg)
{
	msg->m_session_id = overlap->m_session_id;
	msg->m_cmd_id = overlap->m_cmd_id;
	//msg->m_args.setUint64("tran_socket", overlap->m_socket);
	//msg->m_args.setUint64("session_id", overlap->m_session_id);
	//msg->m_args.setUint64("cmd_id", overlap->m_cmd_id);
}




// IocpSvrListenRun -------------------------------------------------------------

IocpUtil::IocpSvrListenRun::IocpSvrListenRun(const MessageLooperNotifyParam & notify_param, socket_t s)
{
	m_notify_param = notify_param;
	m_socket = s;
}

void IocpUtil::IocpSvrListenRun::run()
{
	slog_d("iocp_svr_acceptor:: run --------------------------------");
	__onRun();
	SocketUtil::closeSocket(m_socket);
	slog_d("iocp_svr_acceptor:: exit run --------------------------------");
}

void IocpUtil::IocpSvrListenRun::stop()
{
	m_is_exit = true;
	SocketUtil::closeSocket(m_socket);
}

void IocpUtil::IocpSvrListenRun::__onRun()
{
	while (true)
	{
		if (m_is_exit)
			return;

		slog_d("iocp_svr_acceptor:: start accept");
		socket_t tran_socket = INVALID_SOCKET;
		if (!SocketUtil::accept(m_socket, &tran_socket))
		{
			slog_d("iocp_svr_acceptor:: fail to accept");
			return;
		}

		SocketSelector::Msg_acceptOk* msg = new SocketSelector::Msg_acceptOk();
		msg->m_listen_socket = m_socket;
		msg->m_tran_socket = tran_socket;
		m_notify_param.postMessage(msg);
		slog_d("iocp_svr_acceptor:: accept one client, tran_socket=%0", tran_socket);
	}
}




// IocpTranRun -------------------------------------------------------------

IocpUtil::IocpTranRun::IocpTranRun(HANDLE completionPort, const MessageLooperNotifyParam & notify_param)
{
	m_completionPort = completionPort;
	m_notify_param = notify_param;

}

void IocpUtil::IocpTranRun::run()
{
	slog_d("iocp_svr_tran:: run --------------------------------");
	__onRun();
	slog_d("iocp_svr_tran:: exit run --------------------------------");
}

void IocpUtil::IocpTranRun::stop()
{
	ScopeMutex __l(m_mutex);
	m_is_exit = true;
	IocpUtil::postExitCmd(m_completionPort);
}

void IocpUtil::IocpTranRun::__onRun()
{
	DWORD bytesTransferred = 0;
	ULONG_PTR completionKey = NULL;
	MyOverlap* overlap = NULL;
	bool is_exit = false;
	while (!is_exit)
	{
		bool ret = GetQueuedCompletionStatus(m_completionPort, &bytesTransferred, &completionKey,
			(LPOVERLAPPED *)&overlap, INFINITE);
		slog_d("iocp_svr_tran:: has event **, bytesTransferred=%0", (uint32_t)bytesTransferred);

		if (ret)
		{
			//DWORD flag = 0;
			//if (!WSAGetOverlappedResult(overlap->m_socket, &overlap->m_overlap, &bytesTransferred, FALSE, &flag))
			//{
			//	slog_e("fail to get result");
			//}

			if (overlap->m_cmd_type == EIocpCmdType_exit)
			{
				__onThreadExit();
				is_exit = true;
			}
			else if (overlap->m_cmd_type == EIocpCmdType_send || overlap->m_cmd_type == EIocpCmdType_sendTo)
			{
				if (bytesTransferred == 0)
				{
					__onSocketSendDataEnd(false, overlap, bytesTransferred);
				}
				else
				{
					__onSocketSendDataEnd(true, overlap, bytesTransferred);
				}
			}
			else if (overlap->m_cmd_type == EIocpCmdType_recv || overlap->m_cmd_type == EIocpCmdType_recvFrom)
			{
				if (bytesTransferred == 0)
				{
					__onSocketClose(overlap);
				}
				else
				{
					__onSocketRecvData(overlap, bytesTransferred);
				}
			}
		}
		else
		{
			slog_d("iocp_svr_tran:: ret != 0");

			if ((overlap->m_cmd_type == EIocpCmdType_recv || overlap->m_cmd_type == EIocpCmdType_recvFrom) &&
				overlap != nullptr && bytesTransferred == 0)
			{
				__onSocketClose(overlap);
			}
			else if ((overlap->m_cmd_type == EIocpCmdType_send || overlap->m_cmd_type == EIocpCmdType_sendTo) &&
				overlap != nullptr &&  bytesTransferred == 0)
			{
				__onSocketSendDataEnd(false, overlap, bytesTransferred);
			}
			else
			{
				__onErr();
			}
		}

		IocpUtil::deleteMyOverlap(overlap);
	}
}

void IocpUtil::IocpTranRun::__onErr()
{
	slog_d("iocp_svr_tran:: err event, invalid path");
}

void IocpUtil::IocpTranRun::__onThreadExit()
{
	slog_d("iocp_svr_tran:: exit event");
}

void IocpUtil::IocpTranRun::__onSocketClose(MyOverlap * overlap)
{
	slog_d("iocp_svr_tran:: socket closed event, cmd_type=%0, cmd_id=%1", overlap->m_cmd_type, overlap->m_cmd_id);
	__postSocketClosedMsg(overlap);
}

void IocpUtil::IocpTranRun::__onSocketRecvData(MyOverlap * overlap, uint32_t numberOfBytesTran)
{
	slog_d("iocp_svr_tran:: __onSocketRecvData, recv data event, cmd_id=%0, len=%1", overlap->m_cmd_id, (uint32_t)numberOfBytesTran);

	if (overlap->m_cmd_type == EIocpCmdType_recv)
	{
		__postRecvOkMsg(overlap, numberOfBytesTran);

		if (!IocpUtil::postRecvCmd(overlap->m_socket, overlap->m_session_id, genCmdId()))
		{
			__postSocketClosedMsg(overlap);
		}
	}
	else if (overlap->m_cmd_type == EIocpCmdType_recvFrom)
	{
		__postRecvFromOkMsg(overlap, numberOfBytesTran);

		if (!IocpUtil::postRecvFromCmd(overlap->m_socket, overlap->m_session_id, genCmdId()))
		{
			__postSocketClosedMsg(overlap);
		}
	}
	else
	{
		slog_e("iocp_svr_tran:: __onSocketRecvData, unknown cmd_type=%0", overlap->m_cmd_type);
	}
}

void IocpUtil::IocpTranRun::__onSocketSendDataEnd(bool is_ok, MyOverlap * overlap, uint32_t numberOfBytesTran)
{
	slog_d("iocp_svr_tran:: __onSocketSendDataEnd, send data end event, is_ok=%0, cmd_id=%1, len=%2\n", is_ok, overlap->m_cmd_id, (uint32_t)numberOfBytesTran);
	if (numberOfBytesTran != overlap->m_buffer.len)
	{
		slog_e("IocpTranRun:: __onSocketSendDataEnd, numberOfBytesTran != overlap->m_buffer.len");
	}

	if (overlap->m_cmd_type == EIocpCmdType_send)
		__postSendEndMsg(is_ok, overlap);
	else if (overlap->m_cmd_type == EIocpCmdType_sendTo)
		__postSendToEndMsg(is_ok, overlap);
	else
		slog_e("iocp_svr_tran:: __onSocketSendDataEnd, unknown cmd_type=%0", overlap->m_cmd_type);
}

void IocpUtil::IocpTranRun::__postSocketClosedMsg(MyOverlap * overlap)
{
	SocketSelector::Msg_socketClosed * msg = new SocketSelector::Msg_socketClosed();
	__copyOverlapToMsg(overlap, msg);
	msg->m_socket = overlap->m_socket;
	m_notify_param.postMessage(msg);
}

void IocpUtil::IocpTranRun::__postSendEndMsg(bool is_ok, MyOverlap * overlap)
{
	SocketSelector::Msg_sendEnd* msg = new SocketSelector::Msg_sendEnd();
	__copyOverlapToMsg(overlap, msg);
	msg->m_tran_socket = overlap->m_socket;
	msg->m_is_ok = is_ok;
	m_notify_param.postMessage(msg);
}

void IocpUtil::IocpTranRun::__postRecvOkMsg(MyOverlap * overlap, uint32_t numberOfBytesTran)
{
	SocketSelector::Msg_RecvOk* msg = new SocketSelector::Msg_RecvOk();
	__copyOverlapToMsg(overlap, msg);
	msg->m_tran_socket = overlap->m_socket;
	msg->m_recv_data.append((const byte_t*)overlap->m_buffer.buf, numberOfBytesTran);
	m_notify_param.postMessage(msg);
}

void IocpUtil::IocpTranRun::__postSendToEndMsg(bool is_ok, MyOverlap * overlap)
{
	SocketSelector::Msg_sendToEnd* msg = new SocketSelector::Msg_sendToEnd();
	__copyOverlapToMsg(overlap, msg);
	msg->m_tran_socket = overlap->m_socket;
	msg->m_is_ok = is_ok;
	m_notify_param.postMessage(msg);
}

void IocpUtil::IocpTranRun::__postRecvFromOkMsg(MyOverlap * overlap, uint32_t numberOfBytesTran)
{
	SocketSelector::Msg_RecvFromOk* msg = new SocketSelector::Msg_RecvFromOk();
	__copyOverlapToMsg(overlap, msg);
	msg->m_tran_socket = overlap->m_socket;
	msg->m_recv_data.append((const byte_t*)overlap->m_buffer.buf, numberOfBytesTran);
	SocketUtil::sockaddrToIpAndPort((const sockaddr*)&overlap->m_addr, overlap->m_addr_len, &msg->m_from_ip, &msg->m_from_port);
	m_notify_param.postMessage(msg);
}






#endif
S_NAMESPACE_END
