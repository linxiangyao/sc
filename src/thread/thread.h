#ifndef S_THREAD_H_
#define S_THREAD_H_
#include <thread>
#include "threadComm.h"
#include "../log/log.h"
S_NAMESPACE_BEGIN



enum EThreadState
{
    EThreadState_stop,
    EThreadState_running,
};



class IThreadRun : public IRun
{
public:
    virtual ~IThreadRun() {}
    virtual void stop() = 0;
};



// thread can only start once.
class IThread
{
public:
    static void sleep(int ms);

    virtual ~IThread() {}

    virtual bool            start() = 0;
    virtual void            stop() = 0;
    virtual void            join() = 0;
    virtual void            stopAndJoin() = 0;

    virtual EThreadState    getState() = 0;
    virtual IThreadRun*     getRun() = 0;
	virtual std::thread::native_handle_type getHandle() = 0;
};







class Thread : public IThread
{
public:
	Thread(IThreadRun* r, bool is_auto_delete_run = true);
	~Thread();

	virtual bool start() override;
	virtual void stop() override;
	virtual void join() override;
	virtual void stopAndJoin() override;
	virtual EThreadState getState() override;
	virtual IThreadRun * getRun() override;
	virtual std::thread::native_handle_type getHandle() override;

private:
	void __stop();
	void __join();

	std::thread* m_thread;
	IThreadRun* m_run;
	EThreadState m_thread_state;
	bool m_is_auto_delete_run;
};


S_NAMESPACE_END
#endif //S_NAMESPACE_BEGIN
