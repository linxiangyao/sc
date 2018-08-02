#ifndef S_COMM_H_
#define S_COMM_H_
#include <string>
#include <mutex>
#include "ns.h"
#include "intType.h"
#include "m.h"
#include "binary.h"
#include "variant.h"
#include "stlComm.h"

#if defined(S_OS_WIN)
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <Windows.h>
#else
	#include <sys/time.h>
	#include <time.h>
#endif

S_NAMESPACE_BEGIN



class IRef
{
public:
	virtual void addRef() = 0;
	virtual void subRef() = 0;
	
protected:
	virtual ~IRef() {}
};



class IRun
{
public:
    virtual ~IRun() {}
    virtual void run() = 0;
};




#define S_RELEASE_AND_NULL(c) do{   if(c != NULL) { c->release(); c = NULL; }   }while(0)
#define DELETE_AND_NULL(c) do{   if(c != NULL) { delete c; c = NULL; }   }while(0)



typedef std::unique_lock<std::mutex> ScopeMutex;
typedef std::mutex Mutex;



class SeqIdGenerator
{
public:
	SeqIdGenerator()
	{
		m_id_seed = 0;
	}

	uint64_t genId()
	{
		ScopeMutex l(m_mutex);
		return ++m_id_seed;
	}

private:
	uint64_t m_id_seed;
	Mutex m_mutex;
};


S_NAMESPACE_END
#endif // S_COMM_H_
