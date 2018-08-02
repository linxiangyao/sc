#ifndef S_M_H_
#define S_M_H_



#define __P_CAT(x, y) x##y
#define P_CAT(x, y) __P_CAT(x,y)



#define __P_TO_STR(a) #a
#define P_TO_STR(a) __P_TO_STR(a)



#define P_VARG_WRAP(t)  t



#define __P_ARG_21(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,...)  a21
#define P_ARG_21(...)  P_VARG_WRAP(__P_ARG_21(__VA_ARGS__))
#define P_ARG_NUM(...) P_VARG_WRAP(P_ARG_21(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))



#define P_REPEAT1(marcro) marcro(1)
#define P_REPEAT2(marcro) P_REPEAT1(marcro) marcro(2)
#define P_REPEAT3(marcro) P_REPEAT2(marcro) marcro(3)
#define P_REPEAT4(marcro) P_REPEAT3(marcro) marcro(4)
#define P_REPEAT5(marcro) P_REPEAT4(marcro) marcro(5)
#define P_REPEAT6(marcro) P_REPEAT5(marcro) marcro(6)
#define P_REPEAT7(marcro) P_REPEAT6(marcro) marcro(7)
#define P_REPEAT8(marcro) P_REPEAT7(marcro) marcro(8)
#define P_REPEAT9(marcro) P_REPEAT8(marcro) marcro(9)
#define P_REPEAT10(marcro) P_REPEAT9(marcro) marcro(10)
#define P_REPEAT11(marcro) P_REPEAT10(marcro) marcro(11)
#define P_REPEAT12(marcro) P_REPEAT11(marcro) marcro(12)
#define P_REPEAT13(marcro) P_REPEAT12(marcro) marcro(13)
#define P_REPEAT14(marcro) P_REPEAT13(marcro) marcro(14)
#define P_REPEAT15(marcro) P_REPEAT14(marcro) marcro(15)
#define P_REPEAT16(marcro) P_REPEAT15(marcro) marcro(16)
#define P_REPEAT17(marcro) P_REPEAT16(marcro) marcro(17)
#define P_REPEAT18(marcro) P_REPEAT17(marcro) marcro(18)
#define P_REPEAT19(marcro) P_REPEAT18(marcro) marcro(19)
#define P_REPEAT20(marcro) P_REPEAT19(marcro) marcro(20)
#define P_REPEAT21(marcro) P_REPEAT20(marcro) marcro(21)



#endif


