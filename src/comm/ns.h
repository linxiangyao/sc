#ifndef S_NS_H_
#define S_NS_H_


#ifndef S_NAMESPACE_BEGIN
#define S_NAMESPACE_BEGIN namespace S {
#endif

#ifndef S_NAMESPACE_END
#define S_NAMESPACE_END };
#endif

#ifndef USING_NAMESPACE_S
#define USING_NAMESPACE_S using namespace S;
#endif


#if defined(__MACH__)
#define S_OS_MAC
#elif defined(_WIN32) || defined(_WIN64)
#define S_OS_WIN
#elif defined(__ANDROID__)
#define S_OS_ANDROID
#else
#define S_OS_LINUX
#endif



#ifdef S_OS_WIN
#pragma warning(disable:4996) 
#endif // WIN32


#endif // ! S_N_H_
