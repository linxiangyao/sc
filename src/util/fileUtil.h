#ifndef S_FILEUTIL_H_
#define S_FILEUTIL_H_

#include <string>
#include "../comm/comm.h"

S_NAMESPACE_BEGIN


class FileUtil
{
public:
    static bool readFileAllContent(const std::string& file_path, std::string& content);
	static bool writeFile(const std::string& file_path, const std::string& content);
	static bool deleteFile(const std::string& file_path);
};


S_NAMESPACE_END
#endif

