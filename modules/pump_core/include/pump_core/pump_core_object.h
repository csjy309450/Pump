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
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_noncopyable.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CPrimitiveBase
    : public Pump::Core::CNonCopyable
{
public:
    CPrimitiveBase() {}
    virtual ~CPrimitiveBase() {}
};

class PUMP_CORE_CLASS CObjectBase
{
public:
    CObjectBase();
    CObjectBase(const char* szName, pump_bool_t bIsNull, CPrimitiveBase * pPrimitive=NULL);
    CObjectBase(const CObjectBase& other);
    virtual ~CObjectBase();

public:
    virtual pump_bool_t IsNull() const;
    virtual void SetNull(pump_bool_t bIsNull);
    pump_bool_t operator==(const CObjectBase& other);

public:
    static void* operator new(size_t size);
    static void operator delete(void* ptr);
protected:
    const  char *m_szName;
    pump_bool_t m_bNull;
    CPrimitiveBase * m_pPrimitive; ///< pointer to primitive, but must not free. [Not Owner]
};

class PUMP_CORE_CLASS CNullObject
    : virtual public CObjectBase
{
public:
    CNullObject();
};

class PUMP_CORE_CLASS CNonCopyOperator
{
public:
    CNonCopyOperator();
    virtual ~CNonCopyOperator();
private:
    CNonCopyOperator(const CNonCopyOperator&);
    CNonCopyOperator & operator=(const CNonCopyOperator&);
};

class PUMP_CORE_CLASS CNonNewOperator
{
public:
    CNonNewOperator();
    virtual ~CNonNewOperator();
private:
    static void* operator new(size_t size);
};

class PUMP_CORE_CLASS CObjectMgrBase
{
public:
    CObjectMgrBase() {}
    virtual ~CObjectMgrBase() {}
};

}
}

#endif //PUMP_CORE_OBJCET_H