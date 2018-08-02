#include <stdio.h>
#include "fileUtil.h"
S_NAMESPACE_BEGIN



bool FileUtil::readFileAllContent(const std::string & filePath, std::string & content)
{
    content = "";
    FILE* f = fopen(filePath.c_str(), "rb");
    if (f == NULL)
        return false;

    char buf[2048];
    while (true)
    {
        size_t size = fread(buf, 1, 2048, f);
        if(size != 0)
            content.append(buf, size);
        if (size != 2048)
            break;
    }

    if (ferror(f))
        return false;
    else
        return true;
}

bool FileUtil::writeFile(const std::string & filePath, const std::string & content)
{
	if (content.size() == 0 || filePath.size() == 0)
		return false;

	FILE* f = fopen(filePath.c_str(), "wb+");
	if (f == NULL)
		return false;

	size_t real_write = 0;
	while (true)
	{
		size_t cur_write = fwrite(content.c_str(), 1, content.size(), f);
		if (cur_write == 0)
			break;
			
		real_write += cur_write;
		if (real_write >= content.size())
			break;
	}

	fclose(f);
	return real_write == content.size();
}

bool FileUtil::deleteFile(const std::string& file_path)
{
	int ret = ::remove(file_path.c_str());
	return ret == 0;
}

S_NAMESPACE_END
