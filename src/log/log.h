#ifndef S_LOG_H
#define S_LOG_H
#include <vector>
#include <time.h>
#include <mutex>
#include "logComm.h"
#include "logMarcro.h"
#include "consoleAppender.h"
#include "fileAppender.h"
S_NAMESPACE_BEGIN



/*
 example:
 
 ConsoleAppender* ca = new ConsoleAppender();
 Log::instance().addAppender(ca);
 
 FileAppender* fa = new FileAppender();
 fa->init("", "myfirstlog");
 Log::instance().addAppender(fa);
 
 slog_function_d();
 slog_d("hello, %0, %1, %2\n", "World1", "World2", 1234);
*/
class Log
{
public:
    static Log&   instance();
    static void   releaseInstance();

	void log(const LogInfo& logInfo);
	void addAppender(LogAppender* appender);
	ELogLevel getEnableLogLevel() const;
	void setEnableLogLevel(ELogLevel logLevel);
    
private:
	Log();
	~Log();

    static Log* s_log;
    std::vector<LogAppender*> m_appenders;
    ELogLevel m_enableLogLevel;
    std::mutex m_mutex;
    static std::mutex s_mutex;
};


class ScopeLog
{
public:
	ScopeLog(const char* tag, const char * file, const char* functionName, int line, ELogLevel level);
    ~ScopeLog();

private:
    LogInfo m_logInfo;
	//std::vector<const Variant*> m_vs;
};


#define __WILOG_VARIANT(count) , const LogVariant& v##count

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg);
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT1(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT2(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT3(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT4(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT5(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT6(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT7(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT8(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT9(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT10(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT11(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT12(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT13(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT14(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT15(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT16(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT17(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT18(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT19(__WILOG_VARIANT));
void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT20(__WILOG_VARIANT));



S_NAMESPACE_END
#endif
