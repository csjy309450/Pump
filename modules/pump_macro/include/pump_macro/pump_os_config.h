#ifndef PUMP_OS_CONFIG_H
#define PUMP_OS_CONFIG_H

#if defined (__cplusplus)
#   define PUMP_COMPILER_CXX   1
#endif // (__cplusplus)

#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN64))
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winapifamily.h>
    #define PUMP_OS_WINDOWS   1
    
    // bit long
    #if defined(_WIN64)
        #define PUMP_OS_WIN64 1
    #else 
        #define PUMP_OS_WIN32 1
    #endif // defined(_WIN64)

    #if defined(_WIN32_WCE)
        #define PUMP_OS_WINCE  1
    #endif // defined(_WIN32_WCE)

    #if defined(WINAPI_FAMILY_PARTITION)    ///< avoid warning
        #if (defined(WINAPI_PARTITION_PHONE) && (WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE)))
            #define PUMP_OS_WINPHONE    1 ///< OS win-phone

            #if (_WIN32_WINNT == _WIN32_WINNT_WIN8)
                #define PUMP_OS_WINPHONE8 1 ///< OS win-phone8
            #endif // (_WIN32_WINNT == _WIN32_WINNT_WIN8)
        #endif // (defined(WINAPI_PARTITION_PHONE) && (WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE)))
    #endif // defined(WINAPI_FAMILY_PARTITION)
#elif (defined(__linux__) || defined(__APPLE__) || defined(__SYMBIAN32__) || defined(__ANDROID__))
    #define PUMP_OS_POSIX 1
    
    // bit long
    #if defined(__LP64__)
        #define PUMP_OS_POSIX64   1
    #else 
        #define PUMP_OS_POSIX32   1
    #endif

    #if ((defined(__arm__) || defined(__ARMEL__)) && defined(__linux__))
        #define PUMP_CPU_ARM_LINUX 1
    #endif //((defined(__arm__) || defined(__ARMEL__)) && defined(__linux__))

    #if defined (__APPLE__)
        #define PUMP_OS_APPLE    1
        #if (defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE == 1))
            #define PUMP_OS_IPHONE   1        
        #elif (defined(TARGET_OS_MAC) && (TARGET_OS_MAC == 1))
            #define PUMP_OS_MAC   1
        #endif // (defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE == 1))
    #endif // defined (__APPLE__)

    #if defined(__SYMBIAN32__)
        #define PUMP_OS_S60   1
    #endif // defined(__SYMBIAN32__)

#   if defined(__ANDROID__)
#       define PUMP_OS_ANDROID 1
#   endif // defined(ANDROID)
#else
    #error os not suport yet.
#endif // (defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN64))

#if ((defined PUMP_OS_WIN64) || defined(PUMP_OS_POSIX64))
    #define PUMP_LONG_BIT_64 1
#else
    #define PUMP_LONG_BIT_32 1
#endif // ((defined PUMP_OS_WIN64) || defined(PUMP_OS_POSIX64))

/**
 * [pre-building config] IPv6 support.
 */
#define  PUMP_HAV_IPV6

#ifdef PUMP_HAV_IPV6
#define PUMP_OS_SUPPORT_IPV6 1 ///< IPv6 support.
#endif // PUMP_HAV_IPV6

#endif // PUMP_OS_CONFIG_H