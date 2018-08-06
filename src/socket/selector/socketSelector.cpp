#include "socketSelector.h"
#include "../util/socketUtil.h"
#include "epollUtil.h"
#include "iocpUtil.h"
S_NAMESPACE_BEGIN



SocketSelector::SocketSelector()
{
	slog_d("SocketSelector:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
	m_is_exit = false;
#ifdef S_SELECTOR_IOCP
	m_completionPort = INVALID_HANDLE_VALUE;
#endif
}

SocketSelector::~SocketSelector()
{
	slog_d("SocketSelector:: delete=%0", (uint64_t)this);
	stop();
}

bool SocketSelector::init(MessageLooper* work_looper, const MessageLooperNotifyParam& notify_param)
{
	ScopeMutex __l(m_mutex);
	slog_d("SocketSelector:: init");
	m_work_looper = work_looper;
	m_notify_param = notify_param;

	return true;
}

bool SocketSelector::start()
{
	ScopeMutex __l(m_mutex);
	slog_d("SocketSelector:: start");

#if defined(S_SELECTOR_IOCP)
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	m_completionPort = IocpUtil::createCompletionPort(si.dwNumberOfProcessors);

	//for (size_t i = 0; i < si.dwNumberOfProcessors * 2; ++i)
	{
		Thread* t = new Thread(new IocpUtil::IocpTranRun(m_completionPort, m_notify_param));
		if (!t->start())
		{
			slog_d("SocketSelector:: start fail to t->start"); //TODO: release resource
			return false;
		}
		m_tran_threads.push_back(t);
	}
#elif defined(S_SELECTOR_EPOLL)
	Thread* t = new Thread(new EpollUtil::EpollRun(m_notify_param));
	if (!t->start())
	{
		slog_d("SocketSelector:: start fail to t->start"); //TODO: release resource
		return false;
	}
	m_tran_threads.push_back(t);
#endif

	return true;
}

void SocketSelector::stop()
{
	ScopeMutex __l(m_mutex);
	if (m_is_exit)
		return;
	slog_d("SocketSelector:: stop");
	m_is_exit = true;

	for (auto it = m_tran_sockets.begin(); it != m_tran_sockets.end(); ++it)
	{
		SocketUtil::closeSocket(it->first);
	}
	delete_and_erase_collection_elements(&m_tran_sockets);
	delete_and_erase_collection_elements(&m_tran_threads);
	delete_and_erase_collection_elements(&m_listen_threads);

#if defined(S_SELECTOR_IOCP)
	CloseHandle(m_completionPort);
#elif defined(S_SELECTOR_SELECT)
#endif
}

bool SocketSelector::addTcpSocket(socket_t socket, ETcpSocketType socket_type, uint64_t session_id)
{
	ScopeMutex __l(m_mutex);
	if (m_is_exit)
		return false;
	slog_d("SocketSelector:: addTcpSocket socket=%0, socket_type=%1, session_id=%2", socket, socket_type, session_id);

	if (socket_type == ETcpSocketType_svr_listen)
		return __addAccpetSocket(socket, session_id);
	else
		return __addTranSocket(socket, session_id, true);
}

bool SocketSelector::addUdpSocket(socket_t socket, uint64_t session_id)
{
	ScopeMutex __l(m_mutex);
	if (m_is_exit)
		return false;
	slog_d("SocketSelector:: addUdpSocket socket=%0, session_id=%1", socket, session_id);

	return __addTranSocket(socket, session_id, false);
}

void SocketSelector::removeSocket(socket_t socket, uint64_t session_id)
{
	SocketUtil::closeSocket(socket);
	ScopeMutex __l(m_mutex);
	if (m_is_exit)
		return;

	slog_d("SocketSelector:: removeSocket socket=%0, session_id=%1", socket, session_id);
	delete_and_erase_map_element_by_key(&m_tran_sockets, socket);
}

bool SocketSelector::postSendCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, const byte_t* data, size_t data_len)
{
#ifdef S_SELECTOR_IOCP
	return IocpUtil::postSendCmd(socket, session_id, send_cmd_id, data, data_len);

#elif defined(S_SELECTOR_EPOLL)
	EpollUtil::EpollRun* run = (EpollUtil::EpollRun*)m_tran_threads[0]->getRun();
	return run->postSendCmd(socket, session_id, send_cmd_id, data, data_len);

#else
	return false;
#endif

}

bool SocketSelector::postSendToCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, Ip to_ip, uint32_t to_port, const byte_t * data, size_t data_len)
{
#ifdef S_SELECTOR_IOCP
	return IocpUtil::postSendToCmd(socket, session_id, send_cmd_id, to_ip, to_port, data, data_len);

#elif defined(S_SELECTOR_EPOLL)
	EpollUtil::EpollRun* run = (EpollUtil::EpollRun*)m_tran_threads[0]->getRun();
	return run->postSendToCmd(socket, session_id, send_cmd_id, to_ip, to_port, data, data_len);

#else
	return false;
#endif
}

uint64_t SocketSelector::genSendCmdId()
{
	static Mutex m;
	static uint64_t s_send_cmd_id_seed = 0;
	ScopeMutex __l(m);
	return ++s_send_cmd_id_seed;
}

uint64_t SocketSelector::genSessionId()
{
	static Mutex m;
	static uint64_t s_session_id_seed = 0;
	ScopeMutex __l(m);
	return ++s_session_id_seed;
}

bool SocketSelector::__addTranSocket(socket_t socket, uint64_t session_id, bool is_tcp)
{
#ifdef S_SELECTOR_IOCP
	if (m_tran_sockets.find(socket) != m_tran_sockets.end())
		return true;

	if (!IocpUtil::associateDeviceWithCompletionPort(m_completionPort, (HANDLE)socket, 0))
		return false;

	__SocketCtx* ctx = new __SocketCtx();
	ctx->m_socket = socket;
	m_tran_sockets[socket] = ctx;

	if (is_tcp)
		IocpUtil::postRecvCmd(socket, session_id, IocpUtil::genCmdId());
	else
		IocpUtil::postRecvFromCmd(socket, session_id, IocpUtil::genCmdId());

	return true;

#elif defined(S_SELECTOR_EPOLL)
	EpollUtil::EpollRun* run = (EpollUtil::EpollRun*)m_tran_threads[0]->getRun();
	return run->addTranSocket(socket, session_id, is_tcp);

#else
	return false;
#endif

}

bool SocketSelector::__addAccpetSocket(socket_t socket, uint64_t session_id)
{
#ifdef S_SELECTOR_IOCP
	Thread* listen_thread = new Thread(new IocpUtil::IocpSvrListenRun(m_notify_param, socket));
	listen_thread->start();
	m_listen_threads[socket] = listen_thread;
	return true;

#elif defined(S_SELECTOR_EPOLL)
	EpollUtil::EpollRun* run = (EpollUtil::EpollRun*)m_tran_threads[0]->getRun();
	return run->addAcceptSocket(socket, session_id);

#else
	return false;
#endif
}



S_NAMESPACE_END
