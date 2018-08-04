#ifndef __TEST_STRING_UTIL_H
#define __TEST_STRING_UTIL_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S



void __testStringUtil()
{
	printf("\n__testStringUtil ---------------------------------------------------------\n");
    {
        std::string str = "l";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "ltest";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "testl";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "test l test";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "llll";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "ltestl l testl";
        StringUtil::replace(str, "l", "_");
        cout << "StringUtil::replace str=" << str << endl;

        str = "li";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;

        str = "litest";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;

        str = "testli";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;

        str = "testlitest";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;

        str = "lilili";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;

        str = "li test li li test li";
        StringUtil::replace(str, "li", "_!");
        cout << "StringUtil::replace str=" << str << endl;
    }

    {
        if (!StringUtil::isStringEndWith("hello", "lo"))
        {
            cout << "fail to StringUtil::isStringEndWith" << endl;
        }
    }

    {
        string str = StringUtil::trimBegin(" \r\n\thello");
        if (str != "hello")
        {
            cout << "fail to StringUtil::trimBegin" << endl;
        }
    }

    {
        string str = StringUtil::trimEnd("hello \t\r\n");
        if (str != "hello")
        {
            cout << "fail to StringUtil::trimEnd" << endl;
        }
    }

    {
        string str = StringUtil::trim(" \t\r\nhello \t\r\n");
        if (str != "hello")
        {
            cout << "fail to StringUtil::trim" << endl;
        }
    }


    {
        unsigned int i = 0;
        unsigned int charLength = 0;

        if (!StringUtil::parseUintFromStringBegin("1234xnn", i, charLength))
        {
            cout << "fail to StringUtil::parseUintFromStringBegin(\"1234xnn\")" << endl;
        }
        else
        {
            cout << "StringUtil::parseUintFromStringBegin(\"1234xnn\") ok, i=" << i << ", charLength=" << charLength << endl;
        }
        if (StringUtil::parseUintFromStringBegin("xnn", i, charLength) || i != 0 || charLength != 0)
        {
            cout << "StringUtil::parseUintFromStringBegin(\"xnn\") error" << endl;
        }
        if (StringUtil::parseUintFromStringBegin("", i, charLength) || i != 0 || charLength != 0)
        {
            cout << "StringUtil::parseUintFromStringBegin(\"\") error" << endl;
        }
        if (StringUtil::parseUintFromStringBegin(NULL, i, charLength) || i != 0 || charLength != 0)
        {
            cout << "StringUtil::parseUintFromStringBegin(\"\") error" << endl;
        }
    }
}




#endif
