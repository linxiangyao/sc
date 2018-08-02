#include "./log.h"
#include <iostream>
S_NAMESPACE_BEGIN



Log* Log::s_log = NULL;
std::mutex Log::s_mutex;


Log& Log::instance()
{
    s_mutex.lock();
    if(s_log == NULL)
        s_log = new Log();
    s_mutex.unlock();
    return *s_log;
}

void Log::releaseInstance()
{
    s_mutex.lock();
    if(s_log != NULL)
    {
        delete s_log;
        s_log = NULL;
    }
    s_mutex.unlock();
}



Log::Log() : m_enableLogLevel(ELogLevel_debug)
{
    ScopeMutex _lock(m_mutex);
}

Log::~Log()
{
    ScopeMutex _lock(m_mutex);
    for(size_t i = 0; i < m_appenders.size(); ++i)
    {
        delete m_appenders[i];
    }
}

void Log::log(const LogInfo& logInfo)
{
	ScopeMutex _lock(m_mutex);

	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "Log::log\n");
	if (logInfo.m_logLevel < m_enableLogLevel)
		return;

	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "Log::log 2\n");
	for (size_t i = 0; i < m_appenders.size(); ++i)
	{
		//__android_log_print(ANDROID_LOG_ERROR, "sslib", "Log::log 3\n");
		m_appenders[i]->append(logInfo);
	}
}

void Log::addAppender(LogAppender* appender)
{
	ScopeMutex _lock(m_mutex);
	m_appenders.push_back(appender);
}

ELogLevel Log::getEnableLogLevel() const
{
	return m_enableLogLevel;
}

void Log::setEnableLogLevel(ELogLevel logLevel)
{
    ScopeMutex  _lock(m_mutex);
    m_enableLogLevel = logLevel;
}


#define __WILOG_PUSH_VARIANT(num) vs.push_back(&v##num);


ScopeLog::ScopeLog(const char * tag, const char * file, const char* functionName, int line, ELogLevel level)
{
	__wi_log(tag, file, functionName, line, level, "beg -->");

	m_logInfo.m_tag = tag;
	m_logInfo.m_fileName = file;
	m_logInfo.m_functionName = functionName;
	m_logInfo.m_lineNum = line;
	m_logInfo.m_logLevel = level;
}

ScopeLog::~ScopeLog()
{
    __wi_log(m_logInfo.m_tag.c_str(), m_logInfo.m_fileName.c_str(), m_logInfo.m_functionName.c_str(), m_logInfo.m_lineNum, m_logInfo.m_logLevel, "end <--");
}





void __wi_log(const char * tag, const char * file, const char* functionName, int line, ELogLevel level, const char*msg, std::vector<const LogVariant*>& vs)
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    LogInfo logInfo;
    logInfo.m_tag = tag;
    logInfo.m_fileName = file;
    logInfo.m_functionName = functionName;
    logInfo.m_lineNum = line;
    logInfo.m_logLevel = level;
    time(&logInfo.m_time);
    
    LogFormator f; 
    logInfo.m_msg = f.formatArg(msg, vs);

    Log::instance().log(logInfo);
}

void __wi_log(const char * tag, const char * file, const char* functionName, int line, ELogLevel level, const char * msg)
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT1(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT1(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT2(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT2(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT3(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT3(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT4(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT4(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT5(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT5(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT6(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT6(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT7(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT7(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT8(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT8(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT9(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT9(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT10(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT10(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT11(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT11(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT12(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT12(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT13(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT13(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT14(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT14(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT15(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT15(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT16(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT16(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT17(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT17(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT18(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT18(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT19(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT19(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}

void __wi_log(const char * tag, const char* file, const char* functionName, int line, ELogLevel level, const char* msg P_REPEAT20(__WILOG_VARIANT))
{
	if (level < Log::instance().getEnableLogLevel())
		return;
    std::vector<const LogVariant*> vs;
    P_REPEAT20(__WILOG_PUSH_VARIANT);
    __wi_log(tag, file, functionName, line, level, msg, vs);
}




S_NAMESPACE_END