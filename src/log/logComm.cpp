#include <time.h>
#include <stdio.h>
#include <string.h>
#include "logComm.h"
#include "../util/stringUtil.h"
S_NAMESPACE_BEGIN



std::string LogLevelToOneChar(ELogLevel logLevel)
{
	switch (logLevel)
	{
	case ELogLevel_verbose:
		return std::string() + "V";
	case ELogLevel_debug:
		return std::string() + "D";
	case ELogLevel_info:
		return std::string() + "I";
	case ELogLevel_warning:
		return std::string() + "W";
	case ELogLevel_error:
		return std::string() + "E";
	case ELogLevel_fatal:
		return std::string() + "F";
	}
	return std::string() + "";
}




std::string LogFormator::formatArg(const char * sz, const std::vector<const LogVariant*>& variants)
{
	if (sz == NULL)
		return std::string();

	//if(variants.size() == 0)
	//	return sz;

	std::string str;
	size_t length = strlen(sz);
	for (size_t i = 0; i < length; )
	{
        char c = sz[i];
		if (c == '%')
		{
			if (i < length - 1)
			{
			    if(sz[i + 1] == '%')
			    {
			        str += "%";
			        i = i + 2;
			        continue;
			    }
				unsigned int varintNum = 0;
                unsigned int charLength = 0;
                if (StringUtil::parseUintFromStringBegin(sz + i + 1, varintNum, charLength))
                {
                    if (varintNum < variants.size())
                    {
                        str += variants[varintNum]->toString();
                        i = i + 1 + charLength;
                        continue;
                    }
                }
			}
		}

        ++i;
        str += c;
	}
	return str;
}

std::string LogFormator::formatLog(const LogInfo& logInfo)
{
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog\n");
    // todo: streamappender
    std::string str;
	if(m_formatInfo.m_isShowLevel)
		str.append("[").append(LogLevelToOneChar(logInfo.m_logLevel)).append("]");

	if (m_formatInfo.m_isShowTime)
	{
		tm *tmNow = localtime(&logInfo.m_time);
		std::string strTime;
		if (tmNow != NULL)
		{
			char buf[255];
			memset(buf, 0, 255);
			strftime(buf, 255, "%Y-%m-%d %H:%M:%S", tmNow);
			strTime = buf;
		}
		str.append("[").append(strTime).append("]");
	}
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog 1\n");

    if(m_formatInfo.m_isShowTag)
		str.append("[").append(logInfo.m_tag).append("]");
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog 2\n");
    
    if(m_formatInfo.m_isShowFileName)
		str.append("[").append(logInfo.m_fileName).append("]");
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog 3\n");
    
    if(m_formatInfo.m_isShowLineNum)
		str.append("[").append(StringUtil::toString(logInfo.m_lineNum)).append("]");
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog 4\n");

    if(m_formatInfo.m_isShowFunction)
		str.append("[").append(logInfo.m_functionName).append("]");
	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog 5\n");
    
	if (str.size() == 0)
	{
		str.append(logInfo.m_msg).append("\n");
	}
	else
	{
		str.append(" ").append(logInfo.m_msg).append("\n");
	}

	//__android_log_print(ANDROID_LOG_ERROR, "sslib", "formatLog ok\n");
    return str;
}

void LogFormator::setFormatInfo(const FormatInfo& info)
{
    m_formatInfo = info;
}



S_NAMESPACE_END
