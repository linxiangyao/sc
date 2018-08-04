#ifndef __TEST_P_H
#define __TEST_P_H
#include "../comm.h"
using namespace std;
USING_NAMESPACE_S




#define PP_NUM_PARAMS_0()	,
#define __MYCAT_XXXKJJ "hello, I am __MYCAT_XXXKJJ"
#define __MYCAT_aabb __MYCAT_XXXKJJ
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define FFF(...) P_VARG_WRAP(__FFF(__VA_ARGS__))
#define __FFF(...) P_VARG_WRAP(P_CAT(XXFUN,P_ARG_NUM(__VA_ARGS__))(__VA_ARGS__))
#define XXFUN1(msg) cout << msg << endl
#define XXFUN2(msg,v1) cout << msg << v1 << endl

#define RE1(marcro) marcro(1)
#define RE2(marcro) RE1(marcro),marcro(2)
#define RE3(marcro) RE2(marcro),marcro(3)

#define MYMACRO(v1) int i##v1 = 0;





void __testMarco()
{
	printf("\n__testMarco ---------------------------------------------------------\n");
    //cout << "P_CAT(__MYCAT, _XXXKJJ)=" << P_CAT(__MYCAT, _XXXKJJ) << endl;
    //cout << "P_CAT(__MYCAT, _aabb)=" << P_CAT(__MYCAT, _aabb) << endl;
    //cout << "P_CAT(P_CAT(__MYCAT, _aa), bb)=" << P_CAT(P_CAT(__MYCAT, _aa), bb) << endl;

    cout << "P_ARG_NUM()=" << P_ARG_NUM() << endl;
    cout << "P_ARG_NUM(1)=" << P_ARG_NUM(1) << endl;

    cout << "P_ARG_NUM(,)=" << P_ARG_NUM(, ) << endl;
    cout << "P_ARG_NUM(,1)=" << P_ARG_NUM(, 1) << endl;
    cout << "P_ARG_NUM(1,)=" << P_ARG_NUM(1, ) << endl;
    cout << "P_ARG_NUM(1, 2)=" << P_ARG_NUM(1, 2) << endl;


    cout << "-------------------------" << endl;
    //FFF();
    FFF("hello");
    FFF("hello", 1);
    cout << "P_TO_STR(FFF(\"hello\"))=" << P_TO_STR(FFF("hello")) << endl;
    cout << "P_TO_STR(FFF(\"hello\", 1))=" << P_TO_STR(FFF("hello", 1)) << endl;

    //cout << "-------------------------" << endl;
    //cout << "P_TO_STR(RE3(x))=" << P_TO_STR(RE3(MYMACRO)) << endl;
}



#endif