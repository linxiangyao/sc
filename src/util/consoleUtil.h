#ifndef S_CONSOLE_UTIL_H_
#define S_CONSOLE_UTIL_H_
#include <string>
#include <iostream>
#include "../comm/comm.h"
#include "stringUtil.h"
S_NAMESPACE_BEGIN




class ConsoleUtil
{
public:
    static void pauseConsole()
    {
        pauseConsoleAndGetInput(NULL);
    }
    
    static void pauseConsoleAndGetInput(std::string* strResult)
    {
        while (true)
        {
            char c = 0;
            std::cin.read(&c, 1);
            
            if (strResult != NULL) {
                strResult->append(&c, 1);
            }
            if (c == '\n')
                break;
        }
    }

	static std::map<std::string, std::string> parseArgs(int argc, char** argv)
	{
		std::map<std::string, std::string> r;
		if (argc <= 1)
			return r;

		std::string args = argv[1];
		std::vector<std::string> key_values;
		StringUtil::split(args, ',', &key_values);
		for (size_t i = 0; i < key_values.size(); ++i)
		{
			std::vector<std::string> kv;
			StringUtil::split(key_values[i], '=', &kv);
			if (kv.size() == 0)
				continue;
			r[kv[0]] = kv[1];
		}
		return r;
	}
};





S_NAMESPACE_END
#endif

