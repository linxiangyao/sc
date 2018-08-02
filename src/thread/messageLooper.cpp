#include "../log/log.h"
#include "../util/timeUtil.h"
#include "messageLooper.h"
S_NAMESPACE_BEGIN


std::mutex MessageIdGenerator::s_mutex;

uint32_t MessageIdGenerator::genId()
{
    ScopeMutex _l(s_mutex);
    static uint32_t id = 0;
    return ++id;
}




class MessageLooper::__Event
{
public:
	__Event(Message* msg) : m_msg(msg), m_runnable(nullptr), m_time_out_time(0) {}
	__Event(IRun* r) : m_msg(nullptr), m_runnable(r), m_time_out_time(0) {}
	__Event() : m_msg(nullptr), m_runnable(nullptr), m_time_out_time(0) {}
	~__Event() { delete m_msg; delete m_runnable; }

	Message* m_msg;
	IRun* m_runnable;
	uint64_t m_time_out_time;
};


class MessageLooper::__MsgHandler
{
public:
	__MsgHandler() { m_is_remove = false; m_handler = nullptr; }

	bool m_is_remove;
	IMessageHandler* m_handler;
};


class MessageLooper::__Timer
{
public:
	__Timer() { m_is_running = false; m_id = 0; m_user_data = nullptr; m_delay_ms = 0; m_circle_ms = 0; m_start_time_ms = 0; m_last_callback_time_ms = 0; }
	~__Timer() { }

	void onStart(uint32_t delay_ms, uint32_t circle_ms)
	{
		if (m_is_running)
			return;
		m_is_running = true;

		m_delay_ms = delay_ms;
		m_circle_ms = circle_ms;
		m_start_time_ms = TimeUtil::getMsTime();
		m_last_callback_time_ms = 0;
		//slog_d("__Timer::onStart m_delay_ms=%0, m_circle_ms=%1", m_delay_ms, m_circle_ms);
	}

	void onStop()
	{
		//slog_d("__Timer::onStop");
		m_is_running = false;
		m_delay_ms = 0;
		m_circle_ms = 0;
		m_start_time_ms = 0;
		m_last_callback_time_ms = 0;
	}

	void onTick(uint64_t cur_time_ms, bool* is_cur_need_callback, uint64_t* next_callback_ms)
	{
		//slog_d("onTick m_id=%0, m_is_running=%1, m_delay_ms=%2, m_circle_ms=%3, m_start_time_ms=%4, m_last_callback_time_ms=%5"
		//	, m_id, m_is_running, m_delay_ms, m_circle_ms, m_start_time_ms, m_last_callback_time_ms);

		__onTick(cur_time_ms, is_cur_need_callback, next_callback_ms);
		if (*next_callback_ms <= cur_time_ms)
			slog_e("MessageLooper::__Timer::onTick err! *next_callback_ms <= cur_time_ms");

		//slog_d("onTick m_id=%0, m_is_running=%1, m_delay_ms=%2, m_circle_ms=%3, m_start_time_ms=%4, m_last_callback_time_ms=%5"
		//	, m_id, m_is_running, m_delay_ms, m_circle_ms, m_start_time_ms, m_last_callback_time_ms);
		//slog_d("onTick m_id=%0, start_time_ms=%1, last_callback_ms=%2, next_callback_ms=%3", m_id, m_start_time_ms, m_last_callback_time_ms, *next_callback_ms);
	}

	void __onTick(uint64_t cur_time_ms, bool* is_cur_need_callback, uint64_t* next_callback_ms)
	{
		*is_cur_need_callback = false;
		*next_callback_ms = (uint64_t)-1;

		if (!m_is_running)
			return;

		// once timer
		if (m_circle_ms == 0)
		{
			uint64_t need_callback_ms = m_start_time_ms + m_delay_ms;
			if (cur_time_ms >= need_callback_ms)
			{
				*is_cur_need_callback = true;
				*next_callback_ms = (uint64_t)-1;
				onStop();
			}
			else
			{
				*is_cur_need_callback = false;
				*next_callback_ms = need_callback_ms;
			}
		}

		// circle timer
		else
		{
			uint64_t need_callback_ms = 0;
			if (m_last_callback_time_ms == 0)
			{
				need_callback_ms = m_start_time_ms + m_delay_ms;
			}
			else
			{
				need_callback_ms = m_last_callback_time_ms + m_circle_ms;
			}

			if (cur_time_ms > need_callback_ms)
			{
				*is_cur_need_callback = true;
				m_last_callback_time_ms = need_callback_ms + m_circle_ms * ((cur_time_ms - need_callback_ms) / m_circle_ms);
				*next_callback_ms = m_last_callback_time_ms + m_circle_ms;
			}
			else
			{
				*is_cur_need_callback = false;
				*next_callback_ms = need_callback_ms + m_circle_ms;
			}
		}
	}


	bool m_is_running;
	uint64_t m_id;
	void* m_user_data;
	uint32_t m_delay_ms;
	uint32_t m_circle_ms;
	uint64_t m_start_time_ms;
	uint64_t m_last_callback_time_ms;
};


class MessageLooper::__TimerHandler
{
public:
	__TimerHandler() { m_is_remove = false; m_handler = nullptr; }

	bool m_is_remove;
	IMessageTimerHandler* m_handler;
};


class MessageLooper::__TimerCallbackCtx
{
public:
	__TimerCallbackCtx() { m_timer_id = 0; m_user_data = nullptr; }

	uint64_t m_timer_id;
	void* m_user_data;
};
















// MessageLooper -------------------------------------------------------------------------------

MessageLooper::MessageLooper() : m_is_stop_loop(false), m_mutex()
{
	slog_d("MessageLooper:: new=%0", (uint64_t)this);
	m_timer_id_seed = 0;
	m_cond_wait_ms = -1;
}

MessageLooper::~MessageLooper()
{
	slog_d("MessageLooper:: delete=%0", (uint64_t)this);
    if(!m_is_stop_loop)
    {
        slog_e("!m_is_stop_loop");
    }
	delete_and_erase_collection_elements(&m_id_to_timer_map);
	delete_and_erase_collection_elements(&m_msg_handlers);
	delete_and_erase_collection_elements(&m_timer_handlers);
}

void MessageLooper::loop()
{
    sscope_d();
    while (true)
    {
		std::unique_lock<std::mutex> _l(m_mutex);

		
		__onLoopWakeup();


		if (m_is_stop_loop)
			break;
		if (m_cond_wait_ms == (uint64_t)-1)
		{
			//slog_d("MessageLooper wait forever");
			m_cond.wait(_l);
		}
		else
		{
			//slog_d("MessageLooper wait, m_cond_wait_ms=%0", m_cond_wait_ms);
			m_cond.wait_until(_l, std::chrono::system_clock::now() + std::chrono::milliseconds(m_cond_wait_ms));
		}
    }
}

void MessageLooper::stopLoop()
{
    sscope_d();
    ScopeMutex _l(m_mutex);
    m_is_stop_loop = true;
    m_cond.notify_one();
}

void MessageLooper::addMsgHandler(IMessageHandler* msg_handler)
{
    ScopeMutex _l(m_mutex);
    if(m_is_stop_loop)
        return;

	int index = __findMsgHandlerIndex(msg_handler);
	if (index >= 0)
	{
		m_msg_handlers[index]->m_is_remove = false;
		return;
	}

	__MsgHandler* handler = new __MsgHandler();
	handler->m_handler = msg_handler;
	m_msg_handlers.push_back(handler);
}

void MessageLooper::removeMsgHandler(IMessageHandler* msg_handler)
{
    ScopeMutex _l(m_mutex);
	int index = __findMsgHandlerIndex(msg_handler);
	if (index < 0)
		return;
	m_msg_handlers[index]->m_is_remove = true;
}

void MessageLooper::addMsgTimerHandler(IMessageTimerHandler * timer_handler)
{
	ScopeMutex _l(m_mutex);
	int index = __getTimerHandlerIndexByTimerHandler(timer_handler);
	if (index >= 0)
	{
		m_timer_handlers[index]->m_is_remove = false;
		return;
	}

	__TimerHandler* handler = new __TimerHandler();
	handler->m_handler = timer_handler;
	m_timer_handlers.push_back(handler);	
}

void MessageLooper::removeMsgTimerHandler(IMessageTimerHandler * timer_handler)
{
	ScopeMutex _l(m_mutex);
	int index = __getTimerHandlerIndexByTimerHandler(timer_handler);
	if (index < 0)
		return;
	m_timer_handlers[index]->m_is_remove = true;
}

void MessageLooper::postMessage(Message* msg)
{
    ScopeMutex _l(m_mutex);
	if (m_is_stop_loop)
	{
		delete msg;
		return;
	}

	size_t old_size = m_events.size();
	m_events.push_back(new __Event(msg));
	if (m_events.size() != old_size + 1)
		slog_e("MessageLooper:: postMessage fail to m_events.push_back");

    m_cond.notify_one();
}

void MessageLooper::postMessages(std::list<Message*>* msgs)
{
	ScopeMutex _l(m_mutex);
	if (m_is_stop_loop)
	{
		delete_and_erase_collection_elements(msgs);
		return;
	}

	size_t old_size = m_events.size();
	for (std::list<Message*>::iterator it = msgs->begin(); it != msgs->end(); ++it)
	{
		m_events.push_back(new __Event(*it));
	}
	if (m_events.size() != old_size + msgs->size())
		slog_e("MessageLooper:: postMessages fail to m_events.push_back");
	msgs->clear();

	m_cond.notify_one();
}

void MessageLooper::postMessage(Message* msg, uint32_t delay_ms)
{
	ScopeMutex _l(m_mutex);
	if (m_is_stop_loop)
	{
		delete msg;
		return;
	}

	size_t old_size = m_events.size();
	__Event* e = new __Event(msg);
	e->m_time_out_time = TimeUtil::getMsTime() + delay_ms;
	m_events.push_back(e);
	if (m_events.size() != old_size + 1)
		slog_e("MessageLooper:: postMessage fail to m_events.push_back");

	m_cond.notify_one();
}

void MessageLooper::removeMessageById(uint32_t msg_id)
{
    ScopeMutex _l(m_mutex);
	__EventListIt index = __findMsgIndexById(m_events, msg_id);
	__removeEventByIndex(m_events, index);
}

void MessageLooper::removeMessage(const Message* msg)
{
    ScopeMutex _l(m_mutex);
	__EventListIt index = __findMsgIndex(m_events, msg);
	__removeEventByIndex(m_events, index);
}

void MessageLooper::removeMessagesByMsgType(int msg_type)
{
    ScopeMutex _l(m_mutex);
	for (__EventListIt it = m_events.begin(); it != m_events.end();)
	{
		__EventListIt it_cur = it++;
		Message* msg = (*it_cur)->m_msg;
		if (msg == NULL)
			continue;
		if (msg_type == -1 || msg->m_msg_type == msg_type)
			__removeEventByIndex(m_events, it_cur);
	}
}

void MessageLooper::removeMessagesBySender(void * sender)
{
	removeMessagesBySenderAndMsgType(sender, -1);
}

void MessageLooper::removeMessagesByTarget(void * target)
{
	removeMessagesByTargetAndMsgType(target, -1);
}

void MessageLooper::removeMessagesBySenderAndMsgType(void* sender, int msg_type)
{
    ScopeMutex _l(m_mutex);
	for (__EventListIt it = m_events.begin(); it != m_events.end();)
	{
		__EventListIt it_cur = it++;
		Message* msg = (*it_cur)->m_msg;
		if (msg == NULL)
			continue;
		if (msg->m_sender != sender)
			continue;

		if (msg_type == -1 || msg->m_msg_type == msg_type)
			__removeEventByIndex(m_events, it_cur);
	}
}

void MessageLooper::removeMessagesByTargetAndMsgType(void * target, int msg_type)
{
	ScopeMutex _l(m_mutex);
	for (__EventListIt it = m_events.begin(); it != m_events.end();)
	{
		__EventListIt it_cur = it++;
		Message* msg = (*it_cur)->m_msg;
		if (msg == NULL)
			continue;
		if (msg->m_target != target)
			continue;

		if (msg_type == -1 || msg->m_msg_type == msg_type)
			__removeEventByIndex(m_events, it_cur);
	}
}

bool MessageLooper::hasMessage(const Message* msg)
{
    ScopeMutex _l(m_mutex);
	return __findMsgIndex(m_events, msg) != m_events.end();
}

bool MessageLooper::hasMessageByMsgType(int msgType)
{
    ScopeMutex _l(m_mutex);
	for (__EventListIt it = m_events.begin(); it != m_events.end(); ++it)
	{
		Message* msg =(*it)->m_msg;
		if (msg != NULL && msg->m_msg_type == msgType)
			return true;
	}
	return false;
}

bool MessageLooper::hasMessageBySenderAndMsgType(const void* sender, int msg_type)
{
    ScopeMutex _l(m_mutex);
	for (__EventListIt it = m_events.begin(); it != m_events.end(); ++it)
	{
		Message* msg = (*it)->m_msg;
		if (msg != NULL && msg->m_msg_type == msg_type && msg->m_sender == sender)
			return true;
	}
	return false;
}

uint64_t MessageLooper::createTimer(void * user_data)
{
	ScopeMutex _l(m_mutex);
	__Timer* timer = new __Timer();
	timer->m_user_data = user_data;
	timer->m_id = ++m_timer_id_seed;
	m_id_to_timer_map[timer->m_id] = timer;
	return timer->m_id; 
}

void MessageLooper::releasseTimer(uint64_t timer_id) 
{
	ScopeMutex _l(m_mutex);
	__Timer* timer = __getTimerByTimerId(timer_id);
	if (timer == NULL)
		return;

	delete_and_erase_map_element_by_key(&m_id_to_timer_map, timer_id);
	m_cond.notify_one();
}

bool MessageLooper::startTimer(uint64_t timer_id, uint32_t delay_ms, uint32_t circle_ms)
{
	ScopeMutex _l(m_mutex);
	__Timer* timer = __getTimerByTimerId(timer_id);
	if (timer == NULL)
		return false;

	timer->onStart(delay_ms, circle_ms);
	m_cond.notify_one();
	return true; 
}

void MessageLooper::stopTimer(uint64_t timer_id) 
{
	ScopeMutex _l(m_mutex);
	__Timer* timer = __getTimerByTimerId(timer_id);
	if (timer == NULL)
		return;

	timer->onStop();
	m_cond.notify_one();
}







void MessageLooper::__onLoopWakeup()
{
	while (true)
	{
		__onLoopWakeupEvents();
		__onLoopWakeupTimers();

		if (m_is_stop_loop)
			break;
		if (m_events.size() == 0)
			break;
	}
}

void MessageLooper::__onLoopWakeupEvents()
{
	if (m_is_stop_loop)
		return;

	for (int i = (int)m_msg_handlers.size() - 1; i >= 0; --i)
	{
		if (m_msg_handlers[i]->m_is_remove)
		{
			delete_and_erase_vector_element_by_index(&m_msg_handlers, i);
		}
	}

	// event callback
	__EventList need_callback_events;
	__detachNeedCallbackEvents(&need_callback_events);

	while (need_callback_events.size() > 0)
	{
		__Event* e = *need_callback_events.begin();

		__callbackOneEvent(e); // every thing may be changed

		delete e;
		need_callback_events.erase(need_callback_events.begin());
		if (m_is_stop_loop)
			return;
	}
}

void MessageLooper::__onLoopWakeupTimers()
{
	if (m_is_stop_loop)
		return;

	for (int i = (int)m_timer_handlers.size() - 1; i >= 0; --i)
	{
		if (m_timer_handlers[i]->m_is_remove)
		{
			delete_and_erase_vector_element_by_index(&m_timer_handlers, i);
		}
	}
	
	// timer callback
	std::vector<__TimerCallbackCtx> need_callback_timers;
	__onTickAndGetNeedCallbackTimers(&need_callback_timers);
	while (need_callback_timers.size() > 0)
	{
		__TimerCallbackCtx* t = &*need_callback_timers.begin();

		__callbackOneTimer(t);	// every thing may be changed

		need_callback_timers.erase(need_callback_timers.begin());
		if (m_is_stop_loop)
			return;
	}
}

void MessageLooper::__onTickAndGetNeedCallbackTimers(std::vector<__TimerCallbackCtx>* need_callback_ctxs)
{
	uint64_t cur_time_ms = TimeUtil::getMsTime();
	uint64_t min_next_callback_ms = (uint64_t)-1;
	for (__IdToTimerMap::iterator it = m_id_to_timer_map.begin(); it != m_id_to_timer_map.end(); ++it)
	{
		__Timer* timer = it->second;
		bool is_cur_need_callback = false;
		uint64_t next_callback_ms = -1;

		timer->onTick(cur_time_ms, &is_cur_need_callback, &next_callback_ms);

		min_next_callback_ms = min(next_callback_ms, min_next_callback_ms);
		if (is_cur_need_callback)
		{
			__TimerCallbackCtx ctx;
			ctx.m_timer_id = timer->m_id;
			ctx.m_user_data = timer->m_user_data;
			need_callback_ctxs->push_back(ctx);
		}
	}

	if (min_next_callback_ms == (uint64_t)-1)
	{
		m_cond_wait_ms = -1;
	}
	else
	{
		m_cond_wait_ms = min_next_callback_ms - cur_time_ms;
	}
}





void MessageLooper::__callbackOneEvent(__Event * e)
{
	Message* msg = e->m_msg;
	bool isHandled = false;

	for (size_t i = 0; i < m_msg_handlers.size(); ++i)
	{
		if (m_is_stop_loop)
			break;
		__MsgHandler* handler = m_msg_handlers[i];
		if (handler->m_is_remove)
			continue;

		m_mutex.unlock();
		handler->m_handler->onMessage(msg, &isHandled);
		m_mutex.lock();

		if (isHandled)
			break;
	}
}

void MessageLooper::__callbackOneTimer(__TimerCallbackCtx * t)
{
	for (size_t i = 0; i < m_timer_handlers.size(); ++i)
	{
		if (m_is_stop_loop)
			break;
		__TimerHandler* handler = m_timer_handlers[i];
		if (handler->m_is_remove)
			continue;

		m_mutex.unlock();
		handler->m_handler->onMessageTimerTick(t->m_timer_id, t->m_user_data);
		m_mutex.lock();
	}
}

void MessageLooper::__detachNeedCallbackEvents(__EventList* need_callback_events)
{
	uint64_t curMsTime = TimeUtil::getMsTime();
	for (__EventListIt it = m_events.begin(); it != m_events.end();)
	{
		__EventListIt it_cur = it++;
		__Event* e = (*it_cur);
		if (e->m_time_out_time <= curMsTime)
		{
			need_callback_events->push_back(e);
			m_events.erase(it_cur);
		}
	}
}

int MessageLooper::__findMsgHandlerIndex(const IMessageHandler* msgHandler)
{
	for (size_t i = 0; i < m_msg_handlers.size(); ++i) {
		if (m_msg_handlers[i]->m_handler == msgHandler)
			return (int)i;
	}
	return -1;
}

MessageLooper::__EventListIt MessageLooper::__findMsgIndex(__EventList& events, const Message* msg)
{
	for (__EventListIt it = m_events.begin(); it != m_events.end(); ++it) {
		if ((*it)->m_msg == msg)
			return it;
	}
	return m_events.end();
}

MessageLooper::__EventListIt MessageLooper::__findMsgIndexById(__EventList& events, int msgId)
{
	for (__EventListIt it = m_events.begin(); it != m_events.end(); ++it) {
		if ((*it)->m_msg != NULL && (*it)->m_msg->m_msg_id == msgId)
			return it;
	}
	return m_events.end();
}

MessageLooper::__EventListIt MessageLooper::__findRunnableIndex(__EventList& events, const IRun* r)
{
	for (__EventListIt it = m_events.begin(); it != m_events.end(); ++it) {
		if ((*it)->m_runnable == r)
			return it;
	}
	return m_events.end();
}

void MessageLooper::__removeEventByIndex(__EventList& events, __EventListIt it)
{
	if (it == events.end())
		return;
	delete *it;
	events.erase(it);
}

int MessageLooper::__getTimerHandlerIndexByTimerHandler(IMessageTimerHandler* handler)
{
	for (size_t i = 0; i < m_timer_handlers.size(); ++i)
	{
		if (handler == m_timer_handlers[i]->m_handler)
			return (int)i;
	}
	return -1;
}

MessageLooper::__Timer* MessageLooper::__getTimerByTimerId(uint64_t timer_id)
{
	__IdToTimerMap::iterator it = m_id_to_timer_map.find(timer_id);
	if (it == m_id_to_timer_map.end())
		return NULL;
	return it->second;
}








S_NAMESPACE_END
