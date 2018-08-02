#ifndef S_ITCP_ASYNC_API_H_
#define S_ITCP_ASYNC_API_H_
#include "socketType.h"
#include "iTcpAsyncClientApi.h"
#include "iTcpAsyncServerApi.h"
S_NAMESPACE_BEGIN


/*
tcp async api
*/
class ITcpAsyncApi : public ITcpAsyncClientApi, public ITcpAsyncServerApi
{
};




S_NAMESPACE_END
#endif


