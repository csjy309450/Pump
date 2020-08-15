/**
 * @file __pump_core_time_inner.h
 * @brief Library Inner Header.
 * @details Declaration of inner time interface.
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

#ifndef __PUMP_CORE_TIME_INNER_H
#define __PUMP_CORE_TIME_INNER_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/time/pump_core_time.h"

#define PUMP_USEC_PER_SEC 1000000
#define PUMP_MSEC_PER_SEC 1000
#define PUMP_USEC_PER_MSEC 1000

#define PUMP_DELTA_EPOCH_IN_USEC   pump_time_t(11644473600000000)

#define PUMP_TIME_SEC(time) ((time)/PUMP_USEC_PER_SEC)
#define PUMP_TIME_MSEC(time) (((time)/PUMP_MSEC_PER_SEC)%PUMP_MSEC_PER_SEC)
#define PUMP_TIME_USEC(time) ((time)%PUMP_USEC_PER_SEC)
#define PUMP_TIME_FROM_SEC(sec) ((pump_time_t)(sec)*PUMP_USEC_PER_SEC)
#define PUMP_TIME_MAKE(sec, usec) ((pump_time_t)(sec)*PUMP_USEC_PER_SEC + (pump_time_t)usec)

/** 
 * @brief Leap year is any year divisible by four, but not by 100 unless also
 * divisible by 400
 */
#define PUMP_IS_LEAP_YEAR(year) ((!(year % 4)) ? (((!(year % 400)) && (year % 100)) ? PUMP_TRUE : PUMP_FALSE) : PUMP_FALSE)

#if defined (PUMP_OS_WINDOWS)
void PUMP_CORE_Inner_FileTimeToTime(PUMP_FILETIME * pFileTime, pump_time_t* pHprTime);
void PUMP_CORE_Inner_TimeToFileTime(pump_time_t iHprTime, PUMP_FILETIME * pFileTime);
void PUMP_CORE_Inner_SystemTimeToExpTime(PUMP_SYSTEMTIME * pSysTime, PUMP_TIME_EXP* pExpTime);
#endif // defined (PUMP_OS_WINDOWS)

#endif // __PUMP_CORE_TIME_INNER_H