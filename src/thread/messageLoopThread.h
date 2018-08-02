#ifndef S_MESSAGE_LOOP_THREAD_H_
#define S_MESSAGE_LOOP_THREAD_H_
#include "messageLooper.h"
#include "thread.h"
S_NAMESPACE_BEGIN



class MessageLoopThread : public IThread
{
public:
	MessageLoopThread(IMessageLoopHandler* handler, bool is_auto_delete_handler = true);
    ~MessageLoopThread();

    virtual bool start() override;
    virtual void stop() override;
    virtual void stopAndJoin() override;
    virtual void join() override;
    virtual EThreadState getState() override;
    virtual IThreadRun* getRun() override;
	virtual std::thread::native_handle_type getHandle() override;

    MessageLooper* getLooper();
	IMessageLoopHandler* getMessageLoopHandler();



private:
	class __MessageLoopRunnable;

    IThread* m_thread;
	IMessageLoopHandler* m_handler;
	bool m_is_auto_delete_handler;
    __MessageLoopRunnable* m_runnable;
};





S_NAMESPACE_END
#endif
