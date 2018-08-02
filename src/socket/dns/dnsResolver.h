#ifndef S_DNS_RESOLVER_H_
#define S_DNS_RESOLVER_H_
#include "../base/iSocketApi.h"
S_NAMESPACE_BEGIN


/*
resovle ip text to ip.


NOTE:
	dns typical is a singleton

TODO: 
	1. change to getaddrinfo_a
*/

class DnsResolver : public IMessageHandler, public IDnsResolver
{
public:
	DnsResolver();
	~DnsResolver();

	bool init(MessageLooper* env_loop);

	virtual void stopDnsResolver() override;
	virtual bool addDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target) override;
	virtual void removeDnsResolverNotifyLooper(MessageLooper * notify_loop, void * notify_target) override;
	virtual bool getDnsRecordByName(const std::string & name, DnsRecord * record) override;
	virtual bool startToResolveDns(const std::string & name) override;





private:
	class __WorkRun;


	virtual void onMessage(Message * msg, bool * is_handled) override;


	void __stop();
	void __doResolve();
	void __notifyResolveEnd(bool is_ok, const DnsRecord& record);

	Mutex m_mutex;
	MessageLooper* m_loop;
	std::vector<Thread*> m_resolve_threads;
	std::map<std::string, DnsRecord> m_records;
	std::vector<std::string> m_to_resolve_names;
	MsgLoopNotifySet* m_notify_set;
};




S_NAMESPACE_END
#endif
