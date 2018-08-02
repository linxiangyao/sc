#ifndef S_TCP_SOCKET_API_H_
#define S_TCP_SOCKET_API_H_
#include "socketType.h"
#include "iTcpSyncApi.h"
#include "iTcpAsyncApi.h"
#include "iTcpAsyncClientApi.h"
#include "iTcpAsyncServerApi.h"

#include "iUdpSyncApi.h"
#include "iUdpAsyncApi.h"

#include "iDnsResolver.h"
S_NAMESPACE_BEGIN




bool initSocketLib();
void releaseSocketLib();


/*
socket api


NOTE:
ISocketApi is thread safe.
init(MessageLooper* work_looper)
1. if work_looper != null: ISocketApi will run in the message looper thread which own the work_looper.
2. if work_looper == null: ISocketApi will create a message looper thead and run in the thread.
ITcpAsyncApi will post message to callback_looper.
*/

class ISocketApi : public ITcpSyncApi, public ITcpAsyncApi, public IUdpAsyncApi, public IUdpSyncApi, public IDnsResolver
{
public:
};







S_NAMESPACE_END
#endif

