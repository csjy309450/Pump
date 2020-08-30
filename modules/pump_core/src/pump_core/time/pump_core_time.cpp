/**
 * @file pump_core_time.cpp
 * @brief Library Implementation.
 * @details Implementation of time interface.
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

#include "pump_core/time/pump_core_time.h"
#include "pump_core/time/__pump_core_time_inner.h"

#if (defined PUMP_OS_WINDOWS)
void PUMP_CORE_Inner_FileTimeToTime(PUMP_FILETIME* pFileTime, pump_time_t* pHprTime)
{
#if 0
    * pHprTime = ((Int64ShllMod32(pFileTime->dwHighDateTime, 32) | pFileTime->dwLowDateTime)) / 10;
    *pHprTime -= PUMP_DELTA_EPOCH_IN_USEC;
#else
    /* Convert FILETIME one 64 bit number so we can work with it. */
    *pHprTime = pFileTime->dwHighDateTime;
    *pHprTime = (*pHprTime) << 32;
    *pHprTime |= pFileTime->dwLowDateTime;
    *pHprTime /= 10;    /* Convert from 100 nano-sec periods to micro-seconds. */
    *pHprTime -= PUMP_DELTA_EPOCH_IN_USEC;  /* Convert from Windows epoch to Unix epoch */
#endif
}
#endif // (defined PUMP_OS_WINDOWS)