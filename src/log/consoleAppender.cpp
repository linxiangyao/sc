#include <iostream>
#include "./consoleAppender.h"
S_NAMESPACE_BEGIN


#ifdef S_OS_ANDROID
#include <android/log.h>
#endif



ConsoleAppender::ConsoleAppender()
{

}

ConsoleAppender::~ConsoleAppender()
{

}

void ConsoleAppender::append(const LogInfo& logInfo)
{
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "append\n");

#ifdef S_OS_ANDROID
	if (m_formator.getFormatInfo().m_isShowLevel || m_formator.getFormatInfo().m_isShowTime)
	{
		FormatInfo i = m_formator.getFormatInfo();
		i.m_isShowLevel = false;
		i.m_isShowTime = false;
		m_formator.setFormatInfo(i);
	}
#endif
	std::string str = m_formator.formatLog(logInfo);
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "append ok\n");
	
#ifdef S_OS_ANDROID
	const char* tag = logInfo.m_tag.size() == 0 ? "slib" : logInfo.m_tag.c_str();
	switch (logInfo.m_logLevel)
	{
	case ELogLevel_verbose:
		__android_log_print(ANDROID_LOG_VERBOSE, tag, "%s\n", str.c_str());
		break;
	case ELogLevel_debug:
		__android_log_print(ANDROID_LOG_DEBUG, tag, "%s\n", str.c_str());
		break;
	case ELogLevel_info:
		__android_log_print(ANDROID_LOG_INFO, tag, "%s\n", str.c_str());
		break;
	case ELogLevel_warning:
		__android_log_print(ANDROID_LOG_WARN, tag, "%s\n", str.c_str());
		break;
	case ELogLevel_error:
		__android_log_print(ANDROID_LOG_ERROR, tag, "%s\n", str.c_str());
		break;
	case ELogLevel_fatal:
		__android_log_print(ANDROID_LOG_FATAL, tag, "%s\n", str.c_str());
		break;
	default:
		__android_log_print(ANDROID_LOG_ERROR, tag, "%s\n", str.c_str());
		break;
	}
#else
	std::cout << str;
#endif
}

LogFormator* ConsoleAppender::getFormator()
{
    return &m_formator;
}


S_NAMESPACE_END
