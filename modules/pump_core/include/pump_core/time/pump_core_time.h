/**
 * @file pump_core_time.h
 * @brief Library Header.
 * @details Declaration of time control interface.
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

#ifndef PUMP_CORE_TIME_H
#define PUMP_CORE_TIME_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"

#ifdef PUMP_OS_WINDOWS
#   define PUMP_INFINITE   INFINITE 
#elif (defined PUMP_OS_POSIX)
#   define PUMP_INFINITE   0xffffffff 
#endif // PUMP_OS_WINDOWS

#if defined(PUMP_OS_WINDOWS)
#if !defined PUMP_OS_WINCE
#include <sys/timeb.h>
#endif
typedef SYSTEMTIME    PUMP_SYSTEMTIME;
typedef FILETIME            PUMP_FILETIME;
#elif defined(PUMP_OS_POSIX)
#else
#error os not supported.
#endif

typedef pump_int64_t pump_time_t; ///< (usec)

/**
 * @struct PUMP_TIME_EXP
 */
typedef struct tagPUMP_TIME_EXP
{
    pump_int32_t tm_usec;    ///<    micorseconds past tm_sec
    pump_int32_t tm_sec;    ///<    (0-59)secs past tm_min    
    pump_int32_t tm_min;    ///<    (0-59)minutes past tm_hour
    pump_int32_t tm_hour;    ///<    (0-23)hours past midnight
    pump_int32_t tm_mday;    ///<    (0-31)day of month        
    pump_int32_t tm_mon;    ///<    (0-11)month of year        
    pump_int32_t tm_year;    ///<    year since 1900            
    pump_int32_t tm_wday;    ///<    (0-6)days since sunday    
    pump_int32_t tm_yday;    ///<    (0-365)days since Jan 1    
    pump_int32_t tm_isdst; ///<    daylight saving time    
    pump_int32_t tm_gmtoff;///<    seconds east of UTC        
} PUMP_TIME_EXP;

#endif // PUMP_CORE_TIME_H