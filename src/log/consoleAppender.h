#ifndef S_CONSOLEAPPENDER_H
#define S_CONSOLEAPPENDER_H
#include "logComm.h"
S_NAMESPACE_BEGIN



class ConsoleAppender : public LogAppender
{
public:
    ConsoleAppender();
    ~ConsoleAppender();
    
    void append(const LogInfo& logInfo);
    LogFormator* getFormator();

private:
	LogFormator m_formator;
};



S_NAMESPACE_END
#endif
