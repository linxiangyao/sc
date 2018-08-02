#ifndef S_TIMEUTIL_H_
#define S_TIMEUTIL_H_
#include <string>
#include "../comm/comm.h"
S_NAMESPACE_BEGIN


class TimeUtil
{
public:
    static uint64_t getMsTime();
	static uint64_t getTick();
};


S_NAMESPACE_END
#endif

