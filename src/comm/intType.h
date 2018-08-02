#ifndef S_TYPES_H_
#define S_TYPES_H_


#ifndef WITHOUT_STDINT_H
    #include <stdint.h>
	#include <inttypes.h>
#else
    #ifndef uint8_t
    typedef unsigned char uint8_t;
    #endif

    #ifndef int8_t
    typedef signed char int8_t;
    #endif

    #ifndef uint16_t
    typedef unsigned short uint16_t;
    #endif

    #ifndef int16_t
    typedef signed short int16_t;
    #endif

    #ifndef uint32_t
    typedef unsigned int uint32_t;
    #endif

    #ifndef int32_t
    typedef signed int int32_t;
    #endif

    #ifndef uint64_t
    typedef unsigned long long uint64_t;
    #endif

    #ifndef int64_t
    typedef signed long long int64_t;
    #endif
#endif // WITHOUT_STDINT_H




#ifndef byte_t
typedef uint8_t byte_t;
#endif // byte_t



#endif /* S_TYPES_H_ */
