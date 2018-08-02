#ifndef S_STRINGUTIL_H
#define S_STRINGUTIL_H
#include <string>
#include "../comm/comm.h"
S_NAMESPACE_BEGIN



class StringUtil
{
public:
    static bool         isStringEndWith(const std::string& str, const std::string& endToken);
    static std::string  trimBegin(const std::string& str);
    static std::string  trimEnd(const std::string& str);
    static std::string  trim(const std::string& str);
    static void         replace(std::string& str, const std::string& fromToken, const std::string& toToken);
	static void			split(const std::string& str, const std::vector<char>& splitTokens, std::vector<std::string>* words, bool isTrimEmptyChar = true);
	static void			split(const std::string& str, char splitToken, std::vector<std::string>* words, bool isTrimEmptyChar = true);
	static std::string	fetchMiddle(const std::string& str, const std::string& begin, const std::string& end, bool isTrimEmptyChar = true);

    static bool         isEmptyChar(char c);
	static bool         isNumChar(char c);

	static bool			parseUintFromStringBegin(const char * sz, unsigned int& num, unsigned int& charLength);
    static bool         parseUint(const char*sz, unsigned int* num);
	static unsigned int parseUint(const std::string& str, unsigned int default_value = 0);
	static bool         parseUint64(const char*sz, uint64_t* num);
	static uint64_t		parseUint64(const std::string& str, uint64_t default_value = 0);
    static bool         parseInt(const char*sz, int*num);
	static int			parseInt(const std::string& str, int default_value = 0);

    static std::string  toString(int32_t i);
    static std::string  toString(uint32_t i);
#ifdef S_OS_MAC
	static std::string  toString(size_t i) { return toString((uint32_t)i); }
#endif
	static std::string  toString(int64_t i);
	static std::string  toString(uint64_t i);
	static std::string  toString(const byte_t* bin, size_t len);

	static std::string	byteCountToDisplayString(uint64_t byte_count);
};



S_NAMESPACE_END
#endif
