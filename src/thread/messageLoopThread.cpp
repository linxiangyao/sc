#include "messageLoopThread.h"
#include "../log/log.h"
S_NAMESPACE_BEGIN



// __MessageLoopRunnable -----------------------------------------------------------------------------

class MessageLoopThread::__MessageLoopRunnable : public IThreadRun
{
public:
	__MessageLoopRunnable();
	~__MessageLoopRunnable();

	virtual void run();
	virtual void stop();

	MessageLooper* getLooper();

private:
	MessageLooper m_looper;
};


MessageLoopThread::__MessageLoopRunnable::__MessageLoopRunnable()
{
	//slog_d("new, %0", (uint64_t)this);
}

MessageLoopThread::__MessageLoopRunnable::~__MessageLoopRunnable()
{
	//slog_d("delete, %0", (uint64_t)this);
}

void MessageLoopThread::__MessageLoopRunnable::run()
{
    //sscope_i();
    m_looper.loop();
}

void MessageLoopThread::__MessageLoopRunnable::stop()
{
    m_looper.stopLoop();
}

MessageLooper* MessageLoopThread::__MessageLoopRunnable::getLooper()
{
    return &m_looper;
}













// MessageLoopThread -----------------------------------------------------------------------------

MessageLoopThread::MessageLoopThread(IMessageLoopHandler * logic, bool is_auto_delete_handler)
{
	slog_d("new MessageLoopThread=%0", (uint64_t)this);
	m_runnable = new __MessageLoopRunnable();
	m_thread = new Thread(m_runnable);
	m_handler = logic;
	m_is_auto_delete_handler = is_auto_delete_handler;
	if (m_handler != NULL)
	{
		m_runnable->getLooper()->addMsgHandler(m_handler);
		m_runnable->getLooper()->addMsgTimerHandler(m_handler);
	}
}

MessageLoopThread::~MessageLoopThread()
{
	slog_d("delete MessageLoopThread=%0", (uint64_t)this);
	m_thread->stopAndJoin();

	if (m_handler != NULL)
	{
		m_runnable->getLooper()->removeMsgHandler(m_handler);
		m_runnable->getLooper()->removeMsgTimerHandler(m_handler);
		if(m_is_auto_delete_handler)
			delete m_handler;
	}
	delete m_thread;
	slog_d("delete MessageLoopThread=%0 end", (uint64_t)this);
}

bool MessageLoopThread::start()
{
	slog_d("start");
    return m_thread->start();
}

void MessageLoopThread::stop()
{
	slog_d("stop");
    m_thread->stop();
}

void MessageLoopThread::stopAndJoin()
{
    sscope_d();
    m_thread->stopAndJoin();
}

void MessageLoopThread::join()
{
    sscope_d();
    m_thread->join();
}

EThreadState MessageLoopThread::getState()
{
    return m_thread->getState();
}

IThreadRun* MessageLoopThread::getRun()
{ 
    return m_runnable;
}

std::thread::native_handle_type MessageLoopThread::getHandle()
{
	return m_thread->getHandle();
}

MessageLooper* MessageLoopThread::getLooper()
{
    return m_runnable->getLooper();
}

IMessageLoopHandler* MessageLoopThread::getMessageLoopHandler()
{
	return m_handler;
}



S_NAMESPACE_END
