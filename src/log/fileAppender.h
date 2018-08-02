#ifndef S_FILEAPPENDER_H
#define S_FILEAPPENDER_H
#include <stdio.h>
#include "logComm.h"
#include "../comm/comm.h"
S_NAMESPACE_BEGIN


class FileAppender : public LogAppender
{
public:
	class InitParam
	{
	public:
		InitParam() { m_max_log_day_count = 5; m_max_log_file_size_in_one_day = 40 * 1024 * 1024; m_max_one_log_file_size = m_max_log_file_size_in_one_day / 2; }

		size_t m_max_log_day_count;
		size_t m_max_log_file_size_in_one_day;
		size_t m_max_one_log_file_size;
		bool m_is_zip;
		bool m_is_encrypt;
	};

    FileAppender();
    ~FileAppender();
    
    bool init(const std::string& dir, const std::string& fileName, unsigned int flushLimitSize = 0);
    void append(const LogInfo& logInfo);
    LogFormator* getFormator();
    
private:
    std::string __getTimeStr();

    FILE* m_file;
    std::string m_dir;
    std::string m_fileName;
    LogFormator m_formator;
    size_t m_flushLimitSize;
    size_t m_unflushSize;
};


S_NAMESPACE_END
#endif
