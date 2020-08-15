/**
 * @file pump_core_uuid.h
 * @brief Library Header.
 * @details Declaration of uuid interface.
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

#ifndef PUMP_CORE_UUID_H
#define PUMP_CORE_UUID_H

#define PUMP_UUID_LEN 64

namespace Pump
{
namespace Core
{

class CUUID
{
public:
     explicit CUUID(char chSplit = '-');
    ~CUUID();
    const char* GetUUID() const;
private:
    char m_szUUID[PUMP_UUID_LEN];
};

}
}

#endif // PUMP_CORE_UUID_H
