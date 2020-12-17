/**
 * @file pump_core_multiplex.h
 * @brief Library Header.
 * @details Declaration of multiplex backend interface.
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

#ifndef PUMP_CORE_OS_MULTIPLEX_H
#define PUMP_CORE_OS_MULTIPLEX_H
#include "pump_macro/pump_pre.h"
#include <cstring>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include "pump_core/os_wrapper/pump_core_os_sock.h"

// multiplexing API
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Poll(LPPUMP_POLLFD pFds, pump_int32_t iFds, pump_int32_t iTimeOut);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Select(pump_int32_t iMaxFd, LPPUMP_FDSET fdset_r, LPPUMP_FDSET fdset_w, LPPUMP_FDSET fdset_e, timeval* tv);

#endif //PUMP_CORE_OS_MULTIPLEX_H
