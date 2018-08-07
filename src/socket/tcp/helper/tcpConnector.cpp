#include "tcpConnector.h"
#include "../../util/socketUtil.h"
S_NAMESPACE_BEGIN



#define __TCP_CONNECTOR_MAX_THREAD 5
#define __MSG_TYPE_TcpConnector__WorkRun__connectEnd -28173761


class TcpConnector::__WorkRun : public IThreadRun
{
public:
	__WorkRun(MessageLooper* notify_looper, TcpConnector* notify_target, const __SvrInfo& svr_info)
	{
		m_notify_looper = notify_looper;
		m_notify_target = notify_target;
		m_svr_info = svr_info;
		m_is_exit = false;
	}


	__SvrInfo m_svr_info;


private:
	virtual void run() override
	{
		{
			ScopeMutex __l(m_mutex);
			if (m_is_exit)
				return;
		}

		bool is_ok = SocketUtil::connect(m_svr_info.m_socket, m_svr_info.m_ip, m_svr_info.m_port);
		slog_d("TcpConnector:: connect end, s=%0, is_ok=%1", m_svr_info.m_socket, is_ok);
		__notify(is_ok);

		{
			ScopeMutex __l(m_mutex);
			m_is_exit = true;
		}
	}

	virtual void stop() override
	{
		ScopeMutex __l(m_mutex);
		if (m_is_exit)
			return;

		//m_is_exit = true;
		//slog_d("disconnect socket=%0", m_svr_info.m_socket);
		//SocketUtil::disconnect(m_svr_info.m_socket);
	}

	void __notify(bool is_connected)
	{
		Message* msg = new Message();
		msg->m_msg_type = __MSG_TYPE_TcpConnector__WorkRun__connectEnd;
		msg->m_sender = this;
		msg->m_target = m_notify_target;
		msg->m_args.setUint64("socket", m_svr_info.m_socket);
		msg->m_args.setInt8("is_connected", is_connected);
		m_notify_looper->postMessage(msg);
	}

	MessageLooper* m_notify_looper;
	TcpConnector* m_notify_target;
	bool m_is_exit;
	Mutex m_mutex;
};




TcpConnector::TcpConnector()
{
	slog_d("TcpConnector:: new=%0", (uint64_t)this);
	m_work_loop = nullptr;
	m_notify_loop = nullptr;
	m_notify_target = nullptr;
}

TcpConnector::~TcpConnector()
{
	slog_d("TcpConnector:: delete=%0", (uint64_t)this);
	ScopeMutex _l(m_mutex);
	__stop();
	m_work_loop->removeMsgHandler(this);
}

bool TcpConnector::init(MessageLooper* work_loop, MessageLooper* notify_loop, void* notify_target)
{
	slog_d("TcpConnector:: init");
	ScopeMutex _l(m_mutex);
	m_work_loop = work_loop;
	m_work_loop->addMsgHandler(this);

	m_notify_loop = notify_loop;
	m_notify_target = notify_target;
	return true;
}

void TcpConnector::stopTcpConnector()
{
	slog_d("TcpConnector:: stop");
	ScopeMutex _l(m_mutex);
	__stop();
}

bool TcpConnector::startToConnectTcpSocket(socket_t s, Ip svr_ip, uint32_t svr_port)
{
	ScopeMutex _l(m_mutex);
	slog_d("TcpConnector:: startToConnectTcpSocket, s=%0", s);
	__SvrInfo* info = new __SvrInfo(s, svr_ip, svr_port);
	m_svr_infos.push_back(info);
	__doConnect();
	return true;
}

void TcpConnector::stopConnectTcpSocket(socket_t s)
{
	ScopeMutex _l(m_mutex);
	slog_d("TcpConnector:: stopConnectTcpSocket, s=%0", s);
	for (size_t i = 0; i < m_connect_threads.size(); ++i)
	{
		__WorkRun* run = (__WorkRun*)m_connect_threads[i]->getRun();
		if (run->m_svr_info.m_socket == s)
		{
			delete_and_erase_vector_element_by_index(&m_connect_threads, (int)i);
			break;
		}
	}
}














void TcpConnector::onMessage(Message * msg, bool * is_handled)
{
	ScopeMutex _l(m_mutex);
	if (msg->m_target == this && msg->m_msg_type == __MSG_TYPE_TcpConnector__WorkRun__connectEnd)
	{
		*is_handled = true;
		bool is_connected = msg->m_args.getByte("is_connected");
		socket_t s = msg->m_args.getUint32("socket");

		__notifyConnectEnd(s, is_connected);

		for (size_t i = 0; i < m_connect_threads.size(); ++i)
		{
			if (m_connect_threads[i]->getRun() == msg->m_sender)
			{
				delete_and_erase_vector_element_by_index(&m_connect_threads, (int)i);
				break;
			}
		}

		__doConnect();
	}
}

void TcpConnector::__stop()
{
	delete_and_erase_collection_elements(&m_connect_threads);
	//m_loop->removeMessagesBySender(this);
}

void TcpConnector::__doConnect()
{
	if (m_svr_infos.size() == 0)
		return;
	if (m_connect_threads.size() >= __TCP_CONNECTOR_MAX_THREAD)
		return;

	__SvrInfo info = *m_svr_infos[0];
	delete_and_erase_vector_element_by_index(&m_svr_infos, 0);

	Thread* t = new Thread(new __WorkRun(m_work_loop, this, info));
	m_connect_threads.push_back(t);
	t->start();
}

void TcpConnector::__notifyConnectEnd(socket_t s, bool is_connected)
{
	Msg_ConnecteEnd* msg = new Msg_ConnecteEnd(s, is_connected);
	msg->m_sender = this;
	msg->m_target = m_notify_target;
	m_notify_loop->postMessage(msg);
}




S_NAMESPACE_END
