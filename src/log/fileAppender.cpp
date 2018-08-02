#include <time.h>
#include "fileAppender.h"
#include "../util/stringUtil.h"
S_NAMESPACE_BEGIN




FileAppender::FileAppender():m_file(NULL), m_flushLimitSize(0), m_unflushSize(0)
{

}

FileAppender::~FileAppender()
{
    if(m_file != NULL)
    {
        fclose(m_file);
    }
}

bool FileAppender::init(const std::string & dir, const std::string & fileName, unsigned int flushLimitSize)
{
    m_dir = StringUtil::trim(dir);
    StringUtil::replace(m_dir, "\\", "/");
    if (StringUtil::isStringEndWith(m_dir, "/"))
    {
        m_dir = m_dir.substr(0, m_dir.size() - 1);
    }

    m_fileName = fileName;
    m_flushLimitSize = flushLimitSize;


    std::string timeStr = __getTimeStr();
    std::string realFileName;
    if (m_dir.size() != 0)
    {
        realFileName = m_dir + "/" + m_fileName + "_" + timeStr + ".log";
    }
    else
    {
        realFileName = m_fileName + "_" + timeStr + ".log";
    }

    m_file = fopen(realFileName.c_str(), "ab+");
    if (m_file == NULL)
    {
        //SAssertAndPrint(false, "FileAppender::init fail to fopen %s, dir=%s, fileName=%s", realFileName.c_str(), dir.c_str(), fileName.c_str());
        return false;
    }

    return true;
}

void FileAppender::append(const LogInfo & logInfo)
{
    if (m_file == NULL)
        return;

    std::string str = m_formator.formatLog(logInfo);
    fwrite(str.c_str(), 1, str.size(), m_file);

    m_unflushSize += str.size();
    if (m_unflushSize >= m_flushLimitSize)
    {
        m_unflushSize = 0;
        fflush(m_file);
    }
}

LogFormator* FileAppender::getFormator()
{
    return &m_formator;
}

std::string FileAppender::__getTimeStr()
{
    time_t tNow;
    time(&tNow);
    tm *tmNow = localtime(&tNow);
    std::string strTime;
    if (tmNow != NULL)
    {
        char buf[255];
		memset(buf, 0, 255);
        strftime(buf, 255, "%Y%m%d_%H%M%S", tmNow);
        strTime = buf;
    }
    return strTime;
}



S_NAMESPACE_END
