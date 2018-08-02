#ifndef TEST_DNS_RESOLVER_H
#define TEST_DNS_RESOLVER_H
#include "../comm.h"
#include "../../src/socket/socketLib.h"
using namespace std;
USING_NAMESPACE_S



void __testDnsApi()
{
	initSocketLib();

	// ip --
	{
		struct in_addr ip_v4;
		if (inet_pton(AF_INET, "127.0.0.1", &ip_v4) != 1)
		{
			printf("fail to inet_pton ip_v4\n");
			return;
		}

		in6_addr ip_v6;
		if (inet_pton(AF_INET6, "::FFFF:127.0.0.1", &ip_v6) != 1)
		{
			printf("fail to inet_pton ip_v6\n");
			return;
		}

		char ip_str[100];
		if (inet_ntop(AF_INET, &ip_v4, ip_str, sizeof(ip_str)) == nullptr)
		{
			printf("fail to inet_ntop ip_v4\n");
			return;
		}
		printf("ip=%s\n", ip_str);

		if (inet_ntop(AF_INET6, &ip_v6, ip_str, sizeof(ip_str)) == nullptr)
		{
			printf("fail to inet_ntop ip_v6\n");
			return;
		}
		printf("ip=%s\n", ip_str);
	}


	// addr --
	{
		struct addrinfo addr_hint;
		memset(&addr_hint, 0, sizeof(addr_hint));
		addr_hint.ai_family = AF_INET;
		addr_hint.ai_socktype = SOCK_STREAM;

		struct addrinfo* addr_result = NULL;
		if (getaddrinfo("www.baidu.com", NULL, &addr_hint, &addr_result) != 0)
		{
			printf("fail to getaddrinfo www.baidu.com\n");
			return;
		}

		for (addrinfo* a = addr_result; a != NULL; a = a->ai_next) {
			if (a->ai_family == AF_INET) 
			{
				in_addr ip_v4 = ((sockaddr_in*)a->ai_addr)->sin_addr;
				char ip_str[100];
				if (inet_ntop(AF_INET, &ip_v4, ip_str, sizeof(ip_str)) != nullptr)
					printf("www.baidu.com ip=%s\n", ip_str);
			}
			else if (a->ai_family == AF_INET6)
			{
				//...
			}
		}
	}
}


class __TestDnsResolverLogic : public IConsoleAppLogic
{
private:
	virtual void onAppStartMsg(IConsoleAppApi * api) override
	{
		m_api = api;

		initSocketLib();

		m_dns_resolver = new DnsResolver();
		//if (!m_dns_resolver->init())
		if (!m_dns_resolver->init(&m_api->getMessageLooper()))
		{
			slog_e("fail to init");
			return;
		}
		
		m_dns_resolver->addDnsResolverNotifyLooper(&m_api->getMessageLooper(), this);

		if (!m_dns_resolver->startToResolveDns("163.com"))
		{
			slog_e("fail to startResolve");
			return;
		}
		if (!m_dns_resolver->startToResolveDns("google.com"))
		{
			slog_e("fail to startResolve");
			return;
		}
		if (!m_dns_resolver->startToResolveDns("baidu.com"))
		{
			slog_e("fail to startResolve");
			return;
		}
		if (!m_dns_resolver->startToResolveDns("sdfsdfsaefew.com"))
		{
			slog_e("fail to startResolve");
			return;
		}
	}

	virtual void onAppStopMsg() override
	{
		slog_i("onAppStopMsg begin");
		delete m_dns_resolver;
		slog_i("onAppStopMsg end");
	}

	virtual void onMessage(Message * msg, bool * is_handled) override
	{
		if (msg->m_sender == m_dns_resolver && msg->m_msg_type == DnsResolver::EMsgType_resolveEnd)
		{
			*is_handled = true;
			DnsResolver::Msg_ResolveEnd* m = (DnsResolver::Msg_ResolveEnd*)msg;
			slog_i("Msg_ResolveEnd is_ok=%0, record=%1", m->m_is_ok, __toStr(m->m_record));
		}
	}





	std::string __toStr(const DnsResolver::DnsRecord& record)
	{
		std::string str;
		str = str + "name=" + record.m_name + ", ";
		for (size_t i = 0; i < record.m_ips.size(); ++i)
		{
			const Ip& ip = record.m_ips[i];
			std::string ip_str;
			SocketUtil::ipToStr(ip, &ip_str);
			str = str + "ip=" + ip_str + ", ";
		}
		return str;
	}

	DnsResolver* m_dns_resolver;
	IConsoleAppApi* m_api;
};

void __testDnsResolver()
{
	__initLog(ELogLevel_debug);
	ConsoleApp* app = new ConsoleApp();
	__TestDnsResolverLogic* logic = new __TestDnsResolverLogic();
	app->run(logic);
	delete logic;
	delete app;
}




#endif
