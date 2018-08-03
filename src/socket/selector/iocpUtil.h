#ifndef S_SOCKET_IOCPUTIL_H_
#define S_SOCKET_IOCPUTIL_H_
#include "../base/iSocketApi.h"
#include "socketSelector.h"
S_NAMESPACE_BEGIN



// IocpUtil -------------------------------------------------------------

class IocpUtil
{
private:
	struct MyOverlap;

public:

	static uint64_t genCmdId();
	static HANDLE createCompletionPort(DWORD dwNumberOfConcurrentThreads);
	static BOOL associateDeviceWithCompletionPort(HANDLE hCompletionPort, HANDLE hDevice, DWORD dwCompletionKey);
	static bool postRecvCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id);
	static bool postRecvFromCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id);
	static bool postSendCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id, const byte_t* buf, size_t buf_len);
	static bool postSendToCmd(socket_t socket, uint64_t session_id, uint64_t cmd_id, Ip ip, uint32_t port, const byte_t* buf, size_t buf_len);
	static void postExitCmd(HANDLE completionPort);


	class IocpTranRun : public IThreadRun
	{
	public:
		IocpTranRun(HANDLE completionPort, const MessageLooperNotifyParam& notify_param);
		virtual void run();
		virtual void stop();


	private:
		void __onRun();
		void __onErr();
		void __onThreadExit();
		void __onSocketClose(MyOverlap* overlap);
		void __onSocketRecvData(MyOverlap* overlap, uint32_t numberOfBytesTran);
		void __onSocketSendDataEnd(bool is_ok, MyOverlap* overlap, uint32_t numberOfBytesTran);
		void __postSocketClosedMsg(MyOverlap* overlap);
		void __postSendEndMsg(bool is_ok, MyOverlap* overlap);
		void __postRecvOkMsg(MyOverlap* overlap, uint32_t numberOfBytesTran);
		void __postSendToEndMsg(bool is_ok, MyOverlap* overlap);
		void __postRecvFromOkMsg(MyOverlap* overlap, uint32_t numberOfBytesTran);

		HANDLE m_completionPort;
		MessageLooperNotifyParam m_notify_param;
		bool m_is_exit = false;
		Mutex m_mutex;
	};


	class IocpSvrListenRun : public IThreadRun
	{
	public:
		IocpSvrListenRun(const MessageLooperNotifyParam& notify_param, socket_t s);
		virtual void run() override;
		virtual void stop() override;


	private:
		void __onRun();

		MessageLooperNotifyParam m_notify_param;
		bool m_is_exit = false;
		socket_t m_socket;
	};



private:
	enum EIocpCmdType
	{
		EIocpCmdType_exit = 1,
		EIocpCmdType_recv,
		EIocpCmdType_send,
		EIocpCmdType_recvFrom,
		EIocpCmdType_sendTo,
	};

	struct MyOverlap
	{
		WSAOVERLAPPED m_overlap;
		uint64_t m_cmd_id;
		uint64_t m_session_id;
		EIocpCmdType m_cmd_type;
		socket_t m_socket;
		WSABUF   m_buffer;
		sockaddr_storage m_addr;
		int32_t m_addr_len;
	};


	static MyOverlap* newMyOverlap(socket_t socket, uint64_t session_id, EIocpCmdType cmd_type, uint64_t cmd_id);
	static void deleteMyOverlap(MyOverlap* overlap);
	static void __copyOverlapToMsg(IocpUtil::MyOverlap* overlap, SocketSelector::BaseMsg* msg);
};




S_NAMESPACE_END
#endif
