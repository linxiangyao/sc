#include <string.h>
#include <stdio.h>
#include "stringUtil.h"
S_NAMESPACE_BEGIN



bool StringUtil::isStringEndWith(const std::string& str, const std::string& endToken)
{
    if(str.size() == 0 || endToken.size() == 0 || str.size() < endToken.size())
        return false;
    
    for(size_t i = 0; i < endToken.size(); ++i)
    {
        size_t strPos = str.size() - endToken.size() + i;
        if(endToken[i] != str[strPos])
            return false;
    }
    
    return true;
}

std::string StringUtil::trimBegin(const std::string& str)
{
    if(str.size() == 0)
        return "";
    
    size_t pos = 0;
    for(size_t i = 0; i < str.size(); ++i)
    {
        pos = i;
        if(!isEmptyChar(str[i]))
            break;
    }
    
    return str.substr(pos);
}

std::string StringUtil::trimEnd(const std::string& str)
{
    if(str.size() == 0)
        return "";
    
    int pos = -1;
    for(int i = (int)str.size() - 1; i >= 0; --i)
    {
		if (!isEmptyChar(str[i]))
		{
			pos = i;
			break;
		}
    }
    
    return str.substr(0, pos + 1);
}

std::string StringUtil::trim(const std::string& str)
{
    std::string s;
    s = trimBegin(str);
    s = trimEnd(s);
    return s;
}

void StringUtil::replace(std::string& str, const std::string& fromToken, const std::string& toToken)
{
    size_t cur = 0;
    while (true)
    {
        size_t index = str.find(fromToken, cur);
        if (index == std::string::npos)
            break;

        str.replace(index, fromToken.size(), toToken);
        cur = index + toToken.size();
    }
}

static bool __isCharInVector(char c, const std::vector<char>& cs)
{
	for (size_t i = 0; i < cs.size(); ++i)
	{
		if (c == cs[i])
			return true;
	}
	return false;
}

void StringUtil::split(const std::string& str, const std::vector<char>& splitTokens, std::vector<std::string>* words, bool isTrimEmptyChar)
{
	words->clear();
	if (str.size() == 0)
		return;

	size_t wordBeginIndex = 0;
	for (size_t i = 0; i < str.size(); ++i)
	{
		std::string word;
		if (i == str.size() - 1)
		{
			word = str.substr(wordBeginIndex);
		}
		else
		{
			if (!__isCharInVector(str[i], splitTokens))
				continue;
			word = str.substr(wordBeginIndex, i - wordBeginIndex);
			wordBeginIndex = i + 1;
		}

		if (isTrimEmptyChar)
			trim(word);
		if (word.size() == 0)
			continue;
		words->push_back(word);
	}
}

void StringUtil::split(const std::string& str, char splitToken, std::vector<std::string>* words, bool isTrimEmptyChar)
{
	std::vector<char> splitTokens;
	splitTokens.push_back(splitToken);
	split(str, splitTokens, words, isTrimEmptyChar);
}

std::string StringUtil::fetchMiddle(const std::string& str, const std::string& begin, const std::string& end, bool isTrimEmptyChar)
{
	size_t begin_index = str.find(begin);
	if (begin_index == std::string::npos)
		return "";

	begin_index += begin.size();
	if (end.size() == 0)
		return str.substr(begin_index);

	size_t end_index = str.find(end, begin_index);
	if (end_index == std::string::npos)
		return "";

	std::string r = str.substr(begin_index, end_index - begin_index);
	if (isTrimEmptyChar)
		return trim(r);
	else
		return r;
}

bool StringUtil::isEmptyChar(char c)
{
    if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
        return true;

    return false;
}

bool StringUtil::isNumChar(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

bool StringUtil::parseUintFromStringBegin(const char * sz, unsigned int& num, unsigned int& charLength)
{
    num = 0;
	charLength = 0;

	if (sz == NULL)
		return false;

	size_t length = strlen(sz);
	if (length == size_t(-1))
		return false;

	for (size_t i = 0; i < length; ++i)
	{
		if (!isNumChar(sz[i]))
			break;
		charLength++;
	}

	if (charLength > 64 || charLength == 0)
		return false;
	
	char buf[65];
	memset(buf, 0, 65);
	buf[charLength] = '\0';
	for (size_t i = 0; i < charLength; ++i)
	{
		buf[i] = sz[i];
	}
    sscanf(buf, "%u", &num);

	return true;
}

bool StringUtil::parseUint(const char*sz, unsigned int* num)
{
	*num = 0;
    if (sz == NULL)
        return false;

    if (sscanf(sz, "%u", num) < 0)
        return false;

    return true;
}

unsigned int StringUtil::parseUint(const std::string& str, unsigned int default_value)
{
	unsigned int u = 0;
	if (!parseUint(str.c_str(), &u))
		return default_value;
	return u;
}

bool StringUtil::parseUint64(const char*sz, uint64_t* num)
{
	*num = 0;
	if (sz == NULL)
		return false;

	if (sscanf(sz, "%" PRIu64, num) < 0)
		return false;

	return true;
}

uint64_t StringUtil::parseUint64(const std::string& str, uint64_t default_value)
{
	uint64_t u = 0;
	if (!parseUint64(str.c_str(), &u))
		return default_value;
	return u;
}

bool StringUtil::parseInt(const char*sz, int* num)
{
	*num = 0;
    if (sz == NULL)
        return false;

    if (sscanf(sz, "%d", num) < 0)
        return false;

    return true;
}

int StringUtil::parseInt(const std::string& str, int default_value)
{
	int i = 0;
	if (!parseInt(str.c_str(), &i))
		return default_value;
	return i;
}

std::string StringUtil::toString(int32_t i)
{
    char buf[56];
	memset(buf, 0, 56);
    sprintf(buf, "%d", i);
    return buf;
}

std::string StringUtil::toString(uint32_t i)
{
    char buf[56];
	memset(buf, 0, 56);
    sprintf(buf, "%u", i);
    return buf;
}

std::string StringUtil::toString(int64_t i)
{
	char buf[56];
	memset(buf, 0, 56);
	sprintf(buf, "%" PRIi64, i);
	return buf;
}

std::string StringUtil::toString(uint64_t i)
{
	char buf[56];
	memset(buf, 0, 56);
	sprintf(buf, "%" PRIu64, i);
	return buf;
}

std::string StringUtil::toString(const byte_t* bin, size_t len)
{
	if (bin == NULL || len == 0)
		return "";

	std::string str;
	char* buf = new char[56];
	memset(buf, 0, 56);
	// TODO: string stream
	for (size_t i = 0; i < len; ++i)
	{
		sprintf(buf, "%02x", bin[i]);
		if (i > 0)
			str += " ";
		str += buf;
	}
	return str;
}

std::string StringUtil::byteCountToDisplayString(uint64_t byte_count)
{
	std::string str;
	uint64_t g = byte_count / (1024 * 1024 * 1024);
	uint64_t m = byte_count / (1024 * 1024) % 1024;
	uint64_t k = byte_count / (1024) % 1024;
	uint64_t b = byte_count % 1024;
	if (g > 0)
		str += StringUtil::toString(g) + "G";
	if (m > 0)
	{
		if (str.size() > 0)
			str += ", ";
		str += StringUtil::toString(m) + "M";
	}
	if (k > 0)
	{
		if (str.size() > 0)
			str += ", ";
		str += StringUtil::toString(k) + "K";
	}
	if (b > 0)
	{
		if (str.size() > 0)
			str += ", ";
		str += StringUtil::toString(b) + "B";
	}
	return str;
}


S_NAMESPACE_END
