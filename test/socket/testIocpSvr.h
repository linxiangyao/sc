//#ifndef TEST_IOCP_SVR_H
//#define TEST_IOCP_SVR_H
//#include "../comm.h"
//#include "../../src/socket/socketLib.h"
//#include "../../src/util/utilLib.h"
//using namespace std;
//USING_NAMESPACE_S
//
//
//enum EIocpCmdType
//{
//	EIocpCmdType_exit=  1,
//	EIocpCmdType_recv,
//	EIocpCmdType_send,
//};
//
//#define __OVERLAP_RECV_BUF_LEN (1024 * 10)
//struct MyOverlap
//{
//	WSAOVERLAPPED m_overlap;
//	uint64_t m_cmd_id;
//	EIocpCmdType m_cmd_type;
//
//
//	socket_t m_socket;
//	WSABUF   m_buffer;
//	//DWORD    m_numberOfBytesTran;
//	//DWORD    m_flags;
//};
//
//void __initMyOverlap(MyOverlap* overlap)
//{
//	overlap->m_socket = INVALID_SOCKET;
//	overlap->m_cmd_id = 0;
//	overlap->m_buffer.buf = NULL;
//	overlap->m_buffer.len = 0;
//	overlap->m_cmd_type = EIocpCmdType_exit;
//}
//
//void __deleteMyOverlap(MyOverlap* overlap)
//{
//	delete[] overlap->m_buffer.buf;
//	delete overlap;
//}
//
//HANDLE __createNewCompletionPort(DWORD dwNumberOfConcurrentThreads) {
//	return(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads));
//}
//
//BOOL __associateDeviceWithCompletionPort(HANDLE hCompletionPort, HANDLE hDevice, DWORD dwCompletionKey) {
//	HANDLE h = CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);
//	return h == hCompletionPort;
//}
//
//class __CmdIdGen
//{
//public:
//	static uint64_t genId()
//	{
//		static Mutex m;
//		static uint64_t s_id_seed = 0;
//		ScopeMutex __l(m);
//		return ++s_id_seed;
//	}
//};
//
//bool __postRecvCmd(socket_t socket)
//{
//	MyOverlap* overlap = new MyOverlap();
//	__initMyOverlap(overlap);
//	overlap->m_cmd_id = __CmdIdGen::genId();
//	overlap->m_cmd_type = EIocpCmdType_recv;
//	overlap->m_socket = socket;
//	overlap->m_buffer.buf = new char[__OVERLAP_RECV_BUF_LEN];
//	overlap->m_buffer.len = __OVERLAP_RECV_BUF_LEN;
//	DWORD flag = 0;
//	DWORD numberOfBytesRecvd = 0;
//	int ret = WSARecv(socket, &overlap->m_buffer, 1, &numberOfBytesRecvd, &flag,
//		&overlap->m_overlap, NULL);
//	if (ret != 0)
//	{
//		int err = WSAGetLastError();
//		if (err == WSA_IO_PENDING)
//		{
//			printf("tran:: __postRecvCmd WSA_IO_PENDING, id=%lld\n", overlap->m_cmd_id);
//			return true;
//		}
//		else
//		{
//			printf("tran:: __postRecvCmd fail to WSARecv\n");
//			__deleteMyOverlap(overlap);
//			return false;
//		}
//	}
//	else
//	{
//		printf("tran:: __postRecvCmd already recv, id=%lld, len=%d\n", overlap->m_cmd_id, numberOfBytesRecvd);
//		//byte_t* data = new byte_t[numberOfBytesRecvd + 1];
//		//memset(data, 0, numberOfBytesRecvd + 1);
//		//memcpy(data, overlap->m_buffer.buf, numberOfBytesRecvd);
//		//printf("tran:: __postRecvCmd already recv, id=%lld, len=%d, data=%s\n", overlap->m_cmd_id, numberOfBytesRecvd, (const char*)data);
//	}
//
//	return true;
//}
//
//bool __postSendCmd(socket_t socket, uint64_t cmd_id, const byte_t* buf, size_t buf_len)
//{
//	MyOverlap* overlap = new MyOverlap();
//	__initMyOverlap(overlap);
//	overlap->m_cmd_id = cmd_id;
//	overlap->m_cmd_type = EIocpCmdType_send;
//	overlap->m_socket = socket;
//	overlap->m_buffer.buf = new char[buf_len];
//	memcpy(overlap->m_buffer.buf, buf, buf_len);
//	overlap->m_buffer.len = (ULONG)buf_len;
//	DWORD numOfByteSent = 0;
//
//	int ret = WSASend(socket, &overlap->m_buffer, 1, &numOfByteSent, 0, &overlap->m_overlap, NULL);
//	if (ret != 0)
//	{
//		int err = WSAGetLastError();
//		if (err == WSA_IO_PENDING)
//		{
//			printf("tran:: postSendCmd WSA_IO_PENDING, id=%lld\n", overlap->m_cmd_id);
//			return true;
//		}
//		else
//		{
//			printf("tran:: postSendCmd fail to WSASend\n");
//			__deleteMyOverlap(overlap);
//			return false;
//		}
//	}
//	else
//	{
//		printf("tran:: postSendCmd already sent, id=%lld, len=%d\n", overlap->m_cmd_id, numOfByteSent);
//	}
//
//	return true;
//}
//
//void __postExitCmd(HANDLE completionPort)
//{
//	MyOverlap* overlap = new MyOverlap();
//	__initMyOverlap(overlap);
//	overlap->m_cmd_id = __CmdIdGen::genId();
//	overlap->m_cmd_type = EIocpCmdType_exit;
//	PostQueuedCompletionStatus(completionPort, 0, 0, (LPOVERLAPPED)overlap);
//}
//
//
//
//
//
//
//#define MSG_TYPE_SocketSelector_acceptOneSocket 91278238
//#define MSG_TYPE_SocketSelector_tranRecvData 912783
//#define MSG_TYPE_SocketSelector_tranSendDataEnd 182762371
//#define MSG_TYPE_SocketSelector_tranSocketClosed 1827623727
//
//
//class __TestIocpSvrListenRun : public IThreadRun
//{
//public:
//	__TestIocpSvrListenRun(MessageLooper* notify_loop, void* notify_target)
//	{
//		m_notify_loop = notify_loop;
//		m_notify_target = notify_target;
//	}
//
//	virtual void run()
//	{
//		__onRun();
//		SocketUtil::closeSocket(m_socket);
//		printf("acceptor:: exit run --------------------------------\n");
//	}
//
//	virtual void stop()
//	{
//		m_is_exit = true;
//		SocketUtil::closeSocket(m_socket);
//	}
//
//	void __onRun()
//	{
//		m_socket = SocketUtil::openSocket(ESocketType_tcp);
//		printf("acceptor:: openSocket ok\n");
//
//		if (!SocketUtil::bindAndListen(m_socket, "0.0.0.0", 12306))
//		{
//			printf("svr:: fail to bindAndListen\n");
//			return;
//		}
//
//		printf("acceptor:: bindAndListen ok, port=12306\n");
//
//		while (true)
//		{
//			if (m_is_exit)
//				return;
//
//			printf("acceptor:: start accept\n");
//			socket_t tran_socket = INVALID_SOCKET;
//			if (!SocketUtil::accept(m_socket, &tran_socket))
//			{
//				printf("acceptor:: fail to accept\n");
//				return;
//			}
//
//
//			Message* msg = new Message();
//			msg->m_msg_type = MSG_TYPE_SocketSelector_acceptOneSocket;
//			msg->m_sender = this;
//			msg->m_target = m_notify_target;
//			msg->m_args.setInt64("tran_socket", tran_socket);
//			m_notify_loop->postMessage(msg);
//			printf("acceptor:: accept one client\n");
//		}
//	}
//
//
//private:
//	MessageLooper* m_notify_loop;
//	void* m_notify_target;
//	socket_t m_socket;
//	bool m_is_exit = false;
//};
//
//
//class __TestIocpSvrTranRun : public IThreadRun
//{
//public:
//	__TestIocpSvrTranRun(HANDLE completionPort, MessageLooper* notify_loop, void* notify_target)
//	{
//		m_completionPort = completionPort;
//		m_notify_loop = notify_loop;
//		m_notify_target = notify_target;
//		
//	}
//
//	virtual void run()
//	{
//		printf("tran:: run --------------------------------\n");
//		__onRun();
//		printf("tran:: exit run --------------------------------\n");
//	}
//
//	virtual void stop()
//	{
//		ScopeMutex __l(m_mutex);
//		m_is_exit = true;
//		__postExitCmd(m_completionPort);
//	}
//
//
//private:
//	void __onRun()
//	{
//		DWORD bytesTransferred = 0;
//		ULONG_PTR completionKey = NULL;
//		MyOverlap* overlap = NULL;
//		bool is_exit = false;
//		while (!is_exit)
//		{
//			bool ret = GetQueuedCompletionStatus(m_completionPort, &bytesTransferred, &completionKey,
//				(LPOVERLAPPED *)&overlap, INFINITE);
//			printf("tran:: has event **, bytesTransferred=%d\n", bytesTransferred);
//
//			if (ret)
//			{
//				if (overlap->m_cmd_type == EIocpCmdType_exit)
//				{
//					__onThreadExit();
//					is_exit = true;
//				}
//				else if (overlap->m_cmd_type == EIocpCmdType_recv)
//				{
//					if (bytesTransferred == 0)
//					{
//						__onSocketClose(overlap);
//					}
//					else
//					{
//						__onSocketRecvData(overlap, bytesTransferred);
//					}
//				}
//				else if (overlap->m_cmd_type == EIocpCmdType_send)
//				{
//					if (bytesTransferred == 0)
//					{
//						__onSocketSendDataEnd(false, overlap, bytesTransferred);
//					}
//					else
//					{
//						__onSocketSendDataEnd(true, overlap, bytesTransferred);
//					}
//				}
//			}
//			else
//			{
//				if (overlap != NULL && overlap->m_cmd_type == EIocpCmdType_recv && bytesTransferred == 0)
//				{
//					__onSocketClose(overlap);
//				}
//				else if (overlap != NULL && overlap->m_cmd_type == EIocpCmdType_send && bytesTransferred == 0)
//				{
//					__onSocketSendDataEnd(false, overlap, bytesTransferred);
//				}
//				else
//				{
//					__onErr();
//					//is_exit = true;
//				}
//			}
//
//			__deleteMyOverlap(overlap);
//		}
//	}
//
//	void __onErr()
//	{
//		printf("tran:: err event\n");
//	}
//
//	void __onThreadExit()
//	{
//		printf("tran:: exit event\n");
//	}
//
//	void __onSocketClose(MyOverlap* overlap)
//	{
//		printf("tran:: socket closed event, event_type=%d, id=%lld\n", overlap->m_cmd_type,  overlap->m_cmd_id);
//
//		Message* msg = new Message();
//		msg->m_msg_type = MSG_TYPE_SocketSelector_tranSocketClosed;
//		msg->m_sender = this;
//		msg->m_target = m_notify_target;
//		msg->m_args.setInt64("tran_socket", overlap->m_socket);
//		m_notify_loop->postMessage(msg);
//
//		//{
//		//	ScopeMutex __l(m_mutex);
//		//	delete_and_erase_map_element_by_key(&m_sockets, overlap->m_socket);
//		//}
//		//__startReadCmd(overlap->m_socket);
//	}
//
//	void __onSocketRecvData(MyOverlap* overlap, uint32_t numberOfBytesTran)
//	{
//		printf("tran:: recv data event, id=%lld, len=%d\n", overlap->m_cmd_id, numberOfBytesTran);
//		//Message* msg = new Message();
//		//msg->m_msg_type = MSG_TYPE_SocketSelector_tranRecvData;
//		__postRecvCmd(overlap->m_socket);
//	}
//
//	void __onSocketSendDataEnd(bool is_ok, MyOverlap* overlap, uint32_t numberOfBytesTran)
//	{
//		printf("tran:: send data end event, is_ok=%d, id=%lld, len=%d\n", is_ok, overlap->m_cmd_id, numberOfBytesTran);
//		//DWORD         cbTransfer;
//		//DWORD         dwFlags;
//		//if (WSAGetOverlappedResult(overlap->m_socket, &overlap->m_overlap, &cbTransfer, true, &dwFlags))
//		//{
//		//	int i = 0;
//		//}
//		//else
//		//{
//		//	printf("tran:: send data end event, fail to get result \n");
//		//}
//	}
//
//
//	HANDLE m_completionPort;
//	MessageLooper* m_notify_loop;
//	void* m_notify_target;
//	bool m_is_exit = false;
//	Mutex m_mutex;
//};
//
//
//class __TestIocpSvrTranMgr
//{
//public:
//	__TestIocpSvrTranMgr(MessageLooper* notify_loop, void* notify_target)
//	{
//		m_notify_loop = notify_loop;
//		m_notify_target = notify_target;
//	}
//
//	~__TestIocpSvrTranMgr()
//	{
//		stop();
//	}
//
//	bool start()
//	{
//		ScopeMutex __l(m_mutex);
//		SYSTEM_INFO si;
//		GetSystemInfo(&si);
//
//		m_completionPort = __createNewCompletionPort(si.dwNumberOfProcessors);
//
//		for (size_t i = 0; i < si.dwNumberOfProcessors * 2; ++i)
//		{
//			Thread* t = new Thread(new __TestIocpSvrTranRun(m_completionPort, m_notify_loop, m_notify_target));
//			t->start();
//			m_tran_threads.push_back(t);
//		}
//		return true;
//	}
//
//	void stop()
//	{
//		ScopeMutex __l(m_mutex);
//		if (m_is_exit)
//			return;
//		m_is_exit = true;
//
//		for (auto it = m_sockets.begin(); it != m_sockets.end(); ++it)
//		{
//			SocketUtil::closeSocket(it->first);
//		}
//		delete_and_erase_collection_elements(&m_sockets);
//
//		for (size_t i = 0; i < m_tran_threads.size(); ++i)
//		{
//			m_tran_threads[i]->stop();
//		}
//		delete_and_erase_collection_elements(&m_tran_threads);
//	}
//
//	bool addSocket(socket_t socket)
//	{
//		ScopeMutex __l(m_mutex);
//		if (m_is_exit)
//			return false;
//		if (m_sockets.find(socket) != m_sockets.end())
//			return true;
//
//		if (!__associateDeviceWithCompletionPort(m_completionPort, (HANDLE)socket, 0))
//			return false;
//
//		__SocketCtx* ctx = new __SocketCtx();
//		ctx->m_socket = socket;
//		m_sockets[socket] = ctx;
//
//		//for (size_t i = 0; i < 10; ++i)
//		{
//			__postRecvCmd(socket);
//		}
//		return true;
//	}
//
//	void deleteSocket(socket_t socket)
//	{
//		SocketUtil::closeSocket(socket);
//		ScopeMutex __l(m_mutex);
//		if (m_is_exit)
//			return;
//		delete_and_erase_map_element_by_key(&m_sockets, socket);
//	}
//
//	bool postSendCmd(socket_t socket, uint64_t cmd_id, const byte_t* buf, size_t buf_len)
//	{
//		ScopeMutex __l(m_mutex);
//		if (m_is_exit)
//		{
//			delete[] buf;
//			return false;
//		}
//
//		return __postSendCmd(socket, cmd_id, buf, buf_len);
//	}
//
//
//private:
//	class __SocketCtx
//	{
//	public:
//		socket_t m_socket;
//	};
//
//
//	HANDLE m_completionPort;
//	MessageLooper* m_notify_loop;
//	void* m_notify_target;
//	bool m_is_exit = false;
//	std::map<socket_t, __SocketCtx*> m_sockets;
//	Mutex m_mutex;
//	std::vector<Thread*> m_tran_threads;
//};
//
//
//class __TestIocpSvrLogic : public IConsoleAppLogic
//{
//public:
//	virtual void onAppStartMsg(IConsoleAppApi * api) override
//	{
//		initSocketLib();
//		m_listen_thread = new Thread(new __TestIocpSvrListenRun(&api->getMessageLooper(), this));
//		m_listen_thread->start();
//		m_tran_mgr = new __TestIocpSvrTranMgr(&api->getMessageLooper(), this);
//		m_tran_mgr->start();
//	}
//
//	virtual void onAppStopMsg() override
//	{
//		printf("client:: exiting...\n");
//		delete m_listen_thread;
//		delete m_tran_mgr;
//		releaseSocketLib();
//		printf("client:: exited...\n");
//	}
//
//	virtual void onTextMsg(const std::string & textMsg) override
//	{
//	}
//
//	virtual void onMessage(Message * msg, bool* is_handled) override
//	{
//		if (msg->m_msg_type == MSG_TYPE_SocketSelector_acceptOneSocket && msg->m_target == this)
//		{
//			socket_t tran_socket = msg->m_args.getInt64("tran_socket");
//
//			m_tran_mgr->addSocket(tran_socket);
//
//			//__TestIocpSvrTranRun* run = (__TestIocpSvrTranRun*)(m_tran_thread->getRun());
//			//run->addSocket(tran_socket);
//			//const size_t data_len = 100 * 1000;
//			//byte_t* data = new byte[data_len];
//			//for (size_t i = 0; i < data_len; ++i)
//			//{
//			//	data[i] = 'a';
//			//}
//			//data[data_len - 1] = '\0';
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//			//run->startSendCmd(tran_socket, __CmdIdGen::genId(), data, data_len);
//	/*		run->startSendCmd(tran_socket, __CmdIdGen::genId(), (const byte_t*)"hello", 6);
//			run->startSendCmd(tran_socket, __CmdIdGen::genId(), (const byte_t*)"hello", 6);
//			run->startSendCmd(tran_socket, __CmdIdGen::genId(), (const byte_t*)"hello", 6);
//			run->startSendCmd(tran_socket, __CmdIdGen::genId(), (const byte_t*)"hello", 6);
//			run->startSendCmd(tran_socket, __CmdIdGen::genId(), (const byte_t*)"hello", 6);*/
//		}
//		else if (msg->m_msg_type == MSG_TYPE_SocketSelector_tranSocketClosed && msg->m_target == this)
//		{
//			socket_t tran_socket = msg->m_args.getInt64("tran_socket");
//			m_tran_mgr->deleteSocket(tran_socket);
//		}
//	}
//
//
//
//
//private:
//	uint64_t m_cmd_id_seed = 0;
//	Thread* m_listen_thread;
//	__TestIocpSvrTranMgr* m_tran_mgr;
//};
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//void __testIocpSvr()
//{
//	printf("\n__testIocpSvr ---------------------------------------------------------\n");
//	__initLog(ELogLevel_debug);
//	ConsoleApp* app = new ConsoleApp();
//	__TestIocpSvrLogic* logic = new __TestIocpSvrLogic();
//	app->run(logic);
//	delete logic;
//	delete app;
//}
//
//
//
//
//#endif // !TESTSOCKETSVR_H
//
