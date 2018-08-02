#ifndef S_IDNS_RESOLVER_H_
#define S_IDNS_RESOLVER_H_
#include "socketType.h"
S_NAMESPACE_BEGIN



/*
dns resolver
*/
class IDnsResolver
{
public:
	enum EMsgType
	{
		EMsgType_resolveEnd = 56164781,
	};

	class DnsRecord
	{
	public:
		std::string m_name;
		std::vector<Ip> m_ips;
	};

	class Msg_ResolveEnd : public Message
	{
	public:
		Msg_ResolveEnd(IDnsResolver* resolver, bool is_ok, const DnsRecord& record)
		{
			m_msg_type = EMsgType_resolveEnd;
			m_sender = resolver;
			m_is_ok = is_ok;
			m_record = record;
		}

		bool m_is_ok;
		DnsRecord m_record;
	};


	virtual ~IDnsResolver() {}


	virtual void stopDnsResolver() = 0;
	virtual bool addDnsResolverNotifyLooper(MessageLooper* notify_loop, void* notify_target) = 0;
	virtual void removeDnsResolverNotifyLooper(MessageLooper* notify_loop, void* notify_target) = 0;
	virtual bool getDnsRecordByName(const std::string& name, DnsRecord* record) = 0;
	virtual bool startToResolveDns(const std::string& name) = 0;
};




S_NAMESPACE_END
#endif


