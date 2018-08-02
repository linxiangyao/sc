#include <chrono>
#include "thread.h"
S_NAMESPACE_BEGIN


void IThread::sleep(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//IThread * IThread::newThread(IThreadRun * r)
//{
//	return new Thread(r);
//}


Thread::Thread(IThreadRun * r, bool is_auto_delete_run)
{
	m_run = r;
	m_thread = nullptr;
	m_thread_state = EThreadState_stop;
	m_is_auto_delete_run = is_auto_delete_run;
}

Thread::~Thread()
{
	__stop();
	__join();
	if (m_is_auto_delete_run)
		delete m_run;
	delete m_thread;
}

bool Thread::start()
{
	if (m_thread_state == EThreadState_running)
		return true;

	if (m_thread != nullptr)
		return false;
	m_thread = new std::thread([&]
	{
		m_run->run();
		m_thread_state = EThreadState_stop;
	});

	m_thread_state = EThreadState_running;
	return true;
}

void Thread::stop()
{
	__stop();
}

void Thread::join()
{
	__join();
}

void Thread::stopAndJoin()
{
	__stop();
	__join();
}

EThreadState Thread::getState()
{
	return m_thread_state;
}

IThreadRun * Thread::getRun()
{
	return m_run;
}

std::thread::native_handle_type Thread::getHandle()
{
	if (m_thread == nullptr)
		return std::thread::native_handle_type();
	return m_thread->native_handle();
}

void Thread::__stop()
{
	if (m_thread_state == EThreadState_stop)
		return;
	m_run->stop();
}

void Thread::__join()
{
	if (m_thread == nullptr)
		return;
	if (m_thread->joinable())
		m_thread->join();
	m_thread->native_handle();
}


S_NAMESPACE_END
