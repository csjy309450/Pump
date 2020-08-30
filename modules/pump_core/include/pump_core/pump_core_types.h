/**
 * @file pump_core_types.h
 * @brief Library Header.
 * @details Declaration of data type.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_TYPES_H
#define PUMP_CORE_TYPES_H

#define PUMP_NULL 0
#ifndef PUMP_ERROR
#define PUMP_ERROR -1
#endif // PUMP_ERROR
#ifndef PUMP_OK
#define PUMP_OK 0
#endif // PUMP_OK
#ifndef PUMP_TRUE
#define PUMP_TRUE 1
#endif // !PUMP_TRUE
#ifndef PUMP_FALSE
#define PUMP_FALSE 0
#endif // !PUMP_FALSE

typedef char pump_char_t;
typedef int pump_bool_t;
typedef char pump_int8_t;
typedef unsigned char pump_uint8_t;
typedef unsigned char pump_byte_t;
typedef short pump_int16_t;
typedef short pump_short_t;
typedef unsigned short pump_uint16_t;
typedef short pump_ushort_t;
typedef int pump_int32_t;
typedef unsigned int pump_uint32_t;
#ifdef PUMP_OS_WINDOWS
typedef __int64 pump_int64_t;
typedef unsigned __int64 pump_uint64_t;
#elif (defined PUMP_OS_POSIX)
typedef long long pump_int64_t;
typedef unsigned long long pump_uint64_t;
#endif // PUMP_OS_WINDOWS
typedef long pump_long_t;
typedef unsigned long pump_ulong_t;
typedef char *pump_str_t;
typedef float pump_float32_t;
typedef double pump_float64_t;
typedef void pump_void_t, *pump_pvoid_t;
typedef pump_int64_t pump_size_t;
typedef void* pump_handle_t;

#if (defined PUMP_OS_WINDOWS)
typedef HANDLE pump_fd_t;
#define PUMP_INVALID_SOCKET ((pump_handle_t)INVALID_SOCKET)
#elif (defined PUMP_OS_POSIX)
typedef int pump_fd_t;
#endif // (defined PUMP_OS_WINDOWS)

#define PUMP_INVALID_HANDLE ((pump_handle_t)-1)

#define PUMP_ABSTRACT  ///< sign abstract class
#define PUMP_INTERFACE  ///< sign interface class
#define PUMP_IMPLEMENT  ///<  sign implement class

#define PUMP_ARG_IN ///< sign input parameter
#define PUMP_ARG_OUT ///< sign output parameter
#define PUMP_ARG_IO ///< sign input-output parameter

#endif //PUMP_CORE_TYPES_H
