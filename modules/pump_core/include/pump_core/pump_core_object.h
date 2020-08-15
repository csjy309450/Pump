/**
 * @file pump_core_object.h
 * @brief Library Header.
 * @details Declaration of object system interface.
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

#ifndef PUMP_CORE_OBJCET_H
#define PUMP_CORE_OBJCET_H
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_noncopyable.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CPumpObjectBase
{
public:
    CPumpObjectBase()
        : m_bIsNull(PUMP_TRUE)
        , m_szName("Null")
    {
    }
    CPumpObjectBase(const char* szName, pump_bool_t bIsNull) 
        : m_bIsNull(bIsNull)
        , m_szName(szName) {}
    virtual ~CPumpObjectBase() {}
    virtual pump_bool_t IsNull() const
    {
        return m_bIsNull;
    }
    virtual void SetNull(pump_bool_t bIsNull)
    {
        m_bIsNull = bIsNull;
    }
    pump_bool_t operator==(CPumpObjectBase& other)
    {
        return (::strcmp(other.m_szName, m_szName) == 0);
    }
private:
   const  char *m_szName;
   pump_bool_t m_bIsNull;
};

class PUMP_CORE_CLASS CPumpNull
    : public CPumpObjectBase
{
public:
    CPumpNull() : CPumpObjectBase() {}
};

class PUMP_CORE_CLASS __CPrimitiveBase
    : public Pump::Core::CNonCopyable
{
public:
    __CPrimitiveBase() {}
    virtual ~__CPrimitiveBase() {}
};

class PUMP_CORE_CLASS CObjectBase
{
public:
    CObjectBase(__CPrimitiveBase * pPrimitive);
    virtual ~CObjectBase();
private:
    CObjectBase();
protected:
    __CPrimitiveBase * m_pPrimitive;
};

}
}

#endif //PUMP_CORE_OBJCET_H