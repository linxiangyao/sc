#include "../comm.h"
#include "../../src/util/stringUtil.h"
#include "../../src/util/timeUtil.h"
#include "../../src/thread/threadLib.h"
using namespace std;
USING_NAMESPACE_S



// Runnable -------------------------------------------------------------------------------------
class MyTestRunanble : public IThreadRun
{
public:
    MyTestRunanble() {
        ++id;
        m_id = id;
    }

    virtual void stop()
    {

    }

protected:
    virtual void run()
    {
        printf("Thread %d is running, and start to sleep 1 seconds\n", m_id);
        Thread::sleep(1000);
        printf("Thread %d exit\n", m_id);
    }

    static int id;
    int m_id;
};
int MyTestRunanble::id = 0;


void __testRunnable() 
{
	printf("\n__testRunnable ---------------------------------------------------------\n");
	MyTestRunanble* r = new MyTestRunanble();
	Thread t(r);
	t.start();
	__pauseConsole();
}

// MessageLooper -------------------------------------------------------------------------------------
class MyMsgLoopThreadHandler : public IMessageLoopHandler
{
public:
	MyMsgLoopThreadHandler()
	{
		m_timer_id = 0;
		m_looper = NULL;
	}

	~MyMsgLoopThreadHandler()
	{
		if(m_timer_id != 0)
			m_looper->releasseTimer(m_timer_id);
	}

	void init(MessageLooper* looper)
	{
		m_looper = looper;
	}

private:
	virtual void onMessage(Message * msg, bool * isHandled)
	{
		cout << "loop thread: onMessage, msgType=" << msg->m_msg_type << endl;
		if (msg->m_msg_type == 2)
		{
			cout << "loop thread: create timer, delay_ms = 0, circle ms = 1000" << endl;
			m_timer_id = m_looper->createTimer(nullptr);
			m_looper->startTimer(m_timer_id, 0, 1 * 1000);
		}
	}

	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data)
	{
		if (timer_id != m_timer_id)
			return;

		cout << "loop thread: onMessageTimerTick" << endl;
	}

	uint64_t m_timer_id;
	MessageLooper* m_looper;
};


void __testMsgLoopThread()
{
	printf("\n__testMsgLoopThread ---------------------------------------------------------\n");
	__initLog(ELogLevel_debug);

	MyMsgLoopThreadHandler* h = new MyMsgLoopThreadHandler();
    MessageLoopThread* t = new MessageLoopThread(h, true);
	h->init(t->getLooper());
    t->start();

	{
		__sleepAndPrintAndPauseConsole(10, "main thread: press enter to post message, msg_type=1\n");
		Message* msg = new Message();
		msg->m_msg_type = 1;
		t->getLooper()->postMessage(msg);
	}

	{
		__sleepAndPrintAndPauseConsole(10, "main thread: press enter to post message to createTimer, msg_type=2\n");
		Message* msg = new Message();
		msg->m_msg_type = 2;
		t->getLooper()->postMessage(msg);
	}

	{
		__sleepAndPrintAndPauseConsole(10, "main thread: press enter to exit looper\n");
		t->stopAndJoin();
		delete t;
		__sleepAndPrintAndPauseConsole(10, "main thread: __testMsgLoopThread end\n");
	}
}



class __TestPerformanceMsgLoopThreadHandler : public IMessageLoopHandler
{
public:
	void init(MessageLooper* looper)
	{
		m_looper = looper;
	}

private:
	virtual void onMessage(Message * msg, bool * isHandled)
	{
		if (msg->m_msg_type == -1)
			m_looper->stopLoop();
		//if (msg->m_msg_type % 10000 == 0)
		//{
		//	slog_i("recv msg %0", msg->m_msg_type);
		//}
	}

	virtual void onMessageTimerTick(uint64_t timer_id, void * user_data)
	{
	}

	MessageLooper* m_looper;
};


void __testMsgLoopThreadPerformance()
{
	printf("\n__testPerformanceMsgLoopThread ---------------------------------------------------------\n");
	__initLog(ELogLevel_debug);
	__TestPerformanceMsgLoopThreadHandler* h = new __TestPerformanceMsgLoopThreadHandler();
	MessageLoopThread* t = new MessageLoopThread(h);
	h->init(t->getLooper());
	t->start();

	uint64_t start_ms = TimeUtil::getMsTime();
	int msg_count = 100 * 10000;
	for (int i = 1; i <= msg_count; ++i)
	{
		Message* msg = new Message();
		msg->m_msg_type = i;
		t->getLooper()->postMessage(msg);

		//if (i == msg_count)
		//{
		//	slog_i("post msg=%0", i);
		//}
	}
	uint64_t post_all_msg_end_ms = TimeUtil::getMsTime();

	Message* msg = new Message();
	msg->m_msg_type = -1;
	t->getLooper()->postMessage(msg);
	t->join();
	uint64_t end_ms = TimeUtil::getMsTime();
	delete t;
	uint64_t msg_per_second = msg_count * 1000 / (end_ms - start_ms);
	__sleepAndPrintAndPauseConsole(10, "main thread: msg_count=%i, total_ms=%" PRIu64 ", post_ms=%" PRIu64 ", msg_per_second=%" PRIu64 "\n"
		, msg_count, end_ms - start_ms, post_all_msg_end_ms - start_ms, msg_per_second);
}



void __testThread()
{
	__testRunnable();
	__testMsgLoopThread();
	//__testMutex();
	//__testCondition();
	//__testSem();
	__testMsgLoopThreadPerformance();
}





















































//
//
//// Sem -------------------------------------------------------------------------------------
//class MyTestSemRunnable : public IThreadRun
//{
//public:
//    MyTestSemRunnable(Semaphore* sem) {
//        m_sem = sem;
//        ++id;
//        m_id = id;
//    }
//
//    virtual void stop()
//    {
//
//    }
//
//protected:
//    virtual void run()
//    {
//        printf("Thread %d is running, and wait\n", m_id);
//        m_sem->wait();
//        printf("Thread %d wake up, and start to exit\n", m_id);
//        Thread::sleep(3000);
//        printf("Thread %d exit\n", m_id);
//    }
//
//    static int id;
//    int m_id;
//    Semaphore* m_sem;
//};
//int MyTestSemRunnable::id = 0;
//
//void __testSem()
//{
//    /* std::vector<MyTestSemThread*> threads;
//    Semaphore sem;
//    #ifdef WIN32
//    sem.init(0, 10);
//    #else
//    sem.init(0, 10);
//    #endif // WIN32
//
//    for (int i = 0; i < 10; ++i)
//    {
//    MyTestSemThread* t = new MyTestSemThread(&sem);
//    //t->create();
//    t->start();
//    threads.push_back(t);
//    }
//    Thread::sleep(200);
//    cout << "�����ź�ʹ��5���߳��˳�" << endl;
//    __pauseConsole();
//
//    printf("�ź��ѷ���\n");
//    sem.notify(5);
//    __pauseConsole();*/
//}
//
//
//
//// Mutex -------------------------------------------------------------------------------------
//class MyTestMutexRunnable : public IThreadRun
//{
//public:
//    MyTestMutexRunnable(Mutex* mutex) {
//        m_mutex = mutex;
//        ++id;
//        m_id = id;
//    }
//    virtual void stop()
//    {
//
//    }
//
//protected:
//    void run()
//    {
//        printf("Thread %d try to get lock\n", m_id);
//        m_mutex->lock();
//        printf("Thread %d get lock and sleep 1 second\n", m_id);
//        Thread::sleep(1000);
//        printf("Thread %d release lock and exit\n", m_id);
//        m_mutex->unlock();
//    }
//
//    static int id;
//    int m_id;
//	std::mutex* m_mutex;
//};
//int MyTestMutexRunnable::id = 0;
//
//
//void __testMutex()
//{
//	printf("\n__testMutex ---------------------------------------------------------\n");
//	{
//		printf("case EMutexType_normal ----\n");
//		Mutex m;
//		printf("lock\n");
//		m.lock();
//		printf("unlock\n");
//		m.unlock();
//		printf("lock\n");
//		m.lock();
//		printf("unlock\n");
//		m.unlock();
//	}
//	{
//		printf("case EMutexType_recursive ----\n");
//		Mutex m(EMutexType_recursive);
//		printf("lock\n");
//		m.lock();
//		printf("lock\n");
//		m.lock();
//		printf("unlock\n");
//		m.unlock();
//		printf("unlock\n");
//		m.unlock();
//	}
//
//	printf("case thread ----\n");
//    std::vector<Thread*> threads;
//    Mutex mutex;
//    for (int i = 0; i < 10; ++i)
//    {
//        MyTestMutexRunnable* r = new MyTestMutexRunnable(&mutex);
//        Thread* t = new Thread(r);
//        t->start();
//        threads.push_back(t);
//    }
//    __pauseConsole();
//}
//
//
//
//
//// Condition -------------------------------------------------------------------------------------
//class MyConditionRunnable : public IThreadRun
//{
//public:
//    MyConditionRunnable(Mutex* mutex, Condition* cond) {
//        ++s_id;
//        m_id = s_id;
//        m_mutex = mutex;
//        m_cond = cond;
//    }
//    virtual void stop()
//    {
//
//    }
//
//protected:
//    void run()
//    {
//        printf("Thread %d try to get lock\n", m_id);
//        m_mutex->lock();
//        m_cond->wait(m_mutex);
//		printf("Thread %d get lock, start to sleep 1000 ms\n", m_id);
//		Thread::sleep(1000);
//        m_mutex->unlock();
//        printf("Thread %d relese lock, exit\n", m_id);
//    }
//
//    static int s_id;
//    int m_id;
//	std::mutex* m_mutex;
//	std::condition_variable* m_cond;
//};
//int MyConditionRunnable::s_id = 0;
//
//
//void __testCondition()
//{
//	printf("\n__testCondition ---------------------------------------------------------\n");
//    std::vector<Thread*> threads;
//    Mutex mutex;
//    Condition cond;
//    for (int i = 0; i < 10; ++i)
//    {
//        MyConditionRunnable* r = new MyConditionRunnable(&mutex, &cond);
//        Thread* t = new Thread(r);
//        t->start();
//        threads.push_back(t);
//    }
//
//    Thread::sleep(200);
//    cout << "press to notify one --------" << endl;
//    __pauseConsole();
//    cond.notifyOne();
//
//    Thread::sleep(1100);
//    cout << "press to notify all --------" << endl;
//    __pauseConsole();
//    cond.notifyAll();
//
//    Thread::sleep(10100);
//    cout << "press to exit --------" << endl;
//    __pauseConsole();
//}
//

