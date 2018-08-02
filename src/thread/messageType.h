#ifndef S_MESSAGE_TYPE_H_
#define S_MESSAGE_TYPE_H_
#include "../comm/comm.h"
#include <mutex>
S_NAMESPACE_BEGIN


class MessageIdGenerator
{
public:
	static uint32_t genId();

private:
	static std::mutex s_mutex;
};



/*
if message is simple and is a internal message, you can use m_args to store message fields.
e.g.
	msg.m_args.set("err_code", 1);
 
if message has some complex fileds, or is a public message, you can choice to extend Message.
e.g.
	class MyMsg : public Message
	{
	public:
		virtual Message* clone() const { MyMsg* msg = new MyMsg(); *msg = *this; return msg; }
		ComplexUser m_user;
	};
*/
class Message
{
public:
	Message()
	{
		m_msg_id = MessageIdGenerator::genId();
		m_msg_type = 0;
		m_sender = NULL;
		m_target = NULL;
	}

	virtual ~Message()
	{
	}

	virtual Message* clone() const
	{
		Message* msg = new Message();
		*msg = *this;
		return msg;
	}

	uint32_t m_msg_id;
	int m_msg_type;
	void* m_sender;
	void* m_target;
	VariantMap m_args;
};



class IMessageHandler
{
public:
	virtual ~IMessageHandler() {}
	virtual void onMessage(Message* msg, bool* is_handled) = 0;
};



class IMessageTimerHandler
{
public:
	virtual ~IMessageTimerHandler() {}
	virtual void onMessageTimerTick(uint64_t timer_id, void* user_data) = 0;
};



class IMessageLoopHandler : public IMessageHandler, public IMessageTimerHandler
{
};






S_NAMESPACE_END
#endif
