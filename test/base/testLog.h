#ifndef __TEST_LOG
#define __TEST_LOG

#include "../../src/log/log.h"
#include "../../src/log/consoleAppender.h"
#include "../../src/log/fileAppender.h"
using namespace std;
USING_NAMESPACE_S

void __initLog(ELogLevel log_level)
{
	Log::releaseInstance();
	ConsoleAppender* ca = new ConsoleAppender();
	FormatInfo formatInfo;
	formatInfo.m_isShowFunction = false;
	formatInfo.m_isShowFileName = false;
	formatInfo.m_isShowLineNum = false;
	Log::instance().setEnableLogLevel(log_level);
	ca->getFormator()->setFormatInfo(formatInfo);
	Log::instance().addAppender(ca);
}

void __testWiFormat()
{
	printf("\n__testWiFormat ---------------------------------------------------------\n");
	LogFormator f;
	LogVariant v0((int16_t)10);
	LogVariant v1((int32_t)2048);
	LogVariant v2("hello");
	std::vector<const LogVariant*> v;
	v.push_back(&v0);
	v.push_back(&v1);
	v.push_back(&v2);

	std::string str = f.formatArg("v0=%0, v1=%1, v2=%2", v);
	cout << "format v0=%0, v1=%1, v2=%2, result= " << str << endl;

	str = f.formatArg("v0=%1, v1=%0, v2=%2", v);
	cout << "format v0=%1, v1=%0, v2=%2, result= " << str << endl;

	str = f.formatArg("v0=%0, v0=%0, v2=%2", v);
	cout << "format v0=%0, v0=%0, v2=%2, result= " << str << endl;

	str = f.formatArg("v0=%0, v3=%3, v1=%1", v);
	cout << "format v0=%0, v3=%3, v1=%1, result= " << str << endl;

	str = f.formatArg("v0=%%", v);
	cout << "format v0=%%, v.size=3, result= " << str << endl;

	str = f.formatArg("v0=%%", std::vector<const LogVariant*>());
	cout << "format v0=%%, v.size=0, result= " << str << endl;
}



void __testLog()
{
	__testWiFormat();

	printf("\n__testLog ---------------------------------------------------------\n");
	ConsoleAppender* ca = new ConsoleAppender();
	FormatInfo info;
	info.m_isShowTag = true;
	ca->getFormator()->setFormatInfo(info);
	Log::instance().addAppender(ca);

	FileAppender* fa = new FileAppender();
	fa->init("", "myfirstlog");
	Log::instance().addAppender(fa);

	{
		sscope_d();
		//slog_d();
		slog_d("hello");
		slog_d("hello, %%0");
		slog_d("hello, %0");
		slog_d("hello, %0", "World1");
		slog_d("hello, %0, %1", "World1", "World2");
		slog_d("hello, %1, %0", "World1", "World2");
		slog_d("hello, %0, %1, %0", "World1", "World2");
		slog_d("hello, %0, %1, %2", "World1", "World2", 1234);
		slog_d("hello, %0, %1, %2, %3", "World1", "World2", 1234);

		slog_v("hello");
		slog_i("hello");
		slog_e("hello");
		slog_f("hello");


		slog_v2("NETWORK", "hello");
		slog_d2("NETWORK", "hello");
		slog_i2("NETWORK", "hello");
		slog_e2("NETWORK", "hello");
		slog_f2("NETWORK", "hello");
	}
}



#endif // !__TEST_LOG
