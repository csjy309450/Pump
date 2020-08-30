/**
 * @file pump_core_uuid.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of uuid interface.
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

#include "pump_macro/pump_pre.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#if defined(PUMP_OS_WINDOWS)
#include <objbase.h>
#elif defined(PUMP_OS_POSIX)
#include <uuid/uuid.h>
#else
#error "uuid unsupport platform"
#endif
#include "pump_core/pump_core_uuid.h"

namespace Pump
{
namespace Core
{

CUUID::CUUID(char chSplit)
{
    memset(m_szUUID, 0, sizeof(m_szUUID));
    char szBuff[PUMP_UUID_LEN];
    memset(szBuff, 0, sizeof(szBuff));
#if defined(PUMP_OS_WINDOWS)
    GUID guid;
    if (::CoCreateGuid(&guid))
    {
        return;
    }
    ::sprintf(szBuff,
        "%08X%c%04X%c%04x%c%02X%02X%c%02X%02X%02X%02X%02X%02X",
        guid.Data1, chSplit,
        guid.Data2, chSplit,
        guid.Data3, chSplit,
        guid.Data4[0], guid.Data4[1], chSplit,
        guid.Data4[2], guid.Data4[3], guid.Data4[4], 
        guid.Data4[5], guid.Data4[6], guid.Data4[7], chSplit);
#elif defined(PUMP_OS_POSIX)
    uuid_t uu;
    uuid_generate(uu);

    int32_t index = 0;
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t len = i < 15 ?
            ::sprintf(szBuff + index, "%02X-", uu[i]) :
            ::sprintf(szBuff + index, "%02X", uu[i]);
        if (len < 0)
            return;
        index += len;
    }
#endif // defined(PUMP_OS_WINDOWS)
    memcpy(m_szUUID, szBuff, sizeof(m_szUUID));
}


CUUID::~CUUID()
{
}

const char* CUUID::GetUUID() const
{
    return m_szUUID;
}

}
}
