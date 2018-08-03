#include "socketSelector.h"
#include "../util/socketUtil.h"
#include "iocpUtil.h"
S_NAMESPACE_BEGIN



SocketSelector::SocketSelector()
{
	slog_d("SocketSelector:: new=%0", (uint64_t)this);
	m_work_looper = nullptr;
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

#ifdef S_OS_WIN
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
#else
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
#ifdef S_OS_WIN
	return IocpUtil::postSendCmd(socket, session_id, send_cmd_id, data, data_len);
#else
	return true;
#endif
}

bool SocketSelector::postSendToCmd(socket_t socket, uint64_t session_id, uint64_t send_cmd_id, Ip to_ip, uint32_t to_port, const byte_t * data, size_t data_len)
{
#ifdef S_OS_WIN
	return IocpUtil::postSendToCmd(socket, session_id, send_cmd_id, to_ip, to_port, data, data_len);
#else
	return true;
#endif
}

uint64_t SocketSelector::genSendCmdId()
{
	static Mutex m;
	static uint64_t s_send_cmd_id_seed = 0;
	ScopeMutex __l(m);
	return ++s_send_cmd_id_seed;
}

bool SocketSelector::__addTranSocket(socket_t socket, uint64_t session_id, bool is_tcp)
{
#ifdef S_OS_WIN
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
#else
#endif

	return true;
}

bool SocketSelector::__addAccpetSocket(socket_t socket, uint64_t session_id)
{
#ifdef S_OS_WIN
	Thread* listen_thread = new Thread(new IocpUtil::IocpSvrListenRun(m_notify_param, socket));
	listen_thread->start();
	m_listen_threads[socket] = listen_thread;
#else
#endif
	return true;
}



S_NAMESPACE_END
