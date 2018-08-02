#ifndef __TEST_BINARY_H
#define __TEST_BINARY_H
#include "../src/comm/comm.h"
#include "../src/util/timeUtil.h"
using namespace std;
USING_NAMESPACE_S






void __testBinary()
{
	printf("\n__testBinary ---------------------------------------------------------\n");

	Binary bin;
	bin.append((byte_t)'h');
	bin.append((byte_t)'e');
	bin.append((byte_t)'l');
	bin.append((byte_t)'l');
	bin.append((byte_t)'o');
	bin.append((byte_t)'\0');
	printf("bin data=%s, len=%u, cap=%d\n", (const char*)bin.getData(), (uint32_t)bin.getLen(), (uint32_t)bin.getCap());
	

	uint64_t start_ms = TimeUtil::getMsTime();
	for (int k = 0; k < 100; ++k)
	{
		Binary bin;
#define __BUF_SIZE 100 * 1024
		byte_t buf[__BUF_SIZE];
		for (int i = 0; i < 100; ++i)
		{
			bin.append(buf, __BUF_SIZE);
		}
	}
	uint64_t end_ms = TimeUtil::getMsTime();
	printf("new 100 binaray(100*100K), total_ms=%" PRIu64 "\n", end_ms - start_ms);
}








#endif