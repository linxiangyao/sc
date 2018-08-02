#ifndef __TEST_FILE_UTIL_H
#define __TEST_FILE_UTIL_H
#include <iostream>
#include "../../src/util/stringUtil.h"
#include "../../src/util/fileUtil.h"
using namespace std;
USING_NAMESPACE_S




void __testFileUtil()
{
	printf("\n__testFileUtil ---------------------------------------------------------\n");
    std::string str;
    FileUtil::readFileAllContent("test.txt", str);
    cout << "test.txt content=" << endl << str << endl;
}




#endif
