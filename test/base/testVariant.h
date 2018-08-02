#ifndef __TEST_VARIANT_H
#define __TEST_VARIANT_H

#include "../../src/comm/comm.h"
#include "../../src/util/timeUtil.h"
using namespace std;
USING_NAMESPACE_S



void __testVarint()
{
	printf("\n__testVarint ---------------------------------------------------------\n");
	//long long i = 100;
	//cout << sizeof(long) << endl;
	//cout << sizeof(long long) << endl;
	//cout << sizeof(int) << endl;
	//cout << sizeof(int64_t) << endl;


	Variant v;
	//Variant v2;

	v.setInt32(10);
	cout << "v.setInt32(10)=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setUint16((uint16_t)-1);
	cout << "v.setUint16((uint16_t) -1)=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setInt64((int64_t)9882883992888);
	cout << "v.setInt64((int64_t)9882883992888)=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setFloat((float)2.1);
	cout << "v.setFloat(2.1)=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setDouble((double)2.1);
	cout << "v.setDouble((double)2.1)=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setChar('a');
	cout << "v.setChar('a')=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setUint8((uint8_t)'a');
	cout << "v.setUint8((uint8_t)'a')=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;

	v.setString("hello world");
	cout << "v.setString(\"hello world\")=" << v.toString() << endl;
	//v2 = v;
	//cout << "v2=" << v2.toString() << endl;
}




#endif // !__TEST_VARIANT_H