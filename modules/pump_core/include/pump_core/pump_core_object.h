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
#include "pump_core/pump_core_nonop.h"

namespace Pump
{
namespace Core
{

class CNonCopyOperator;
class CNonNewOperator;
class CObjectBase;

typedef enum tagPUMP_CORE_ALLOC_TYPE
{
    PUMP_CORE_ALLOC_BUILDIN = 0,
    PUMP_CORE_ALLOC_MEMPOLL,
} PUMP_CORE_ALLOC_TYPE;

class PUMP_CORE_CLASS CObjectPrivateBase
    : public CNonCopyOperator
{
public:
    friend class CObjectBase;
public:
    CObjectPrivateBase();
    virtual ~CObjectPrivateBase();

    PUMP_CORE_ALLOC_TYPE GetAllocType() const;
    size_t GetSize() const;
    pump_bool_t IsHeap() const;
public:
    void* operator new(size_t size);
    void* operator new(size_t size, PUMP_CORE_ALLOC_TYPE emType);
    void operator delete(void* ptr);
    void operator delete(void* ptr, PUMP_CORE_ALLOC_TYPE emType);
private:
    PUMP_CORE_ALLOC_TYPE m_emType;
    size_t m_size;
};

class PUMP_CORE_CLASS CObjectBase
{
public:
    CObjectBase();
    CObjectBase(const char* szName, pump_bool_t bIsNull, CObjectPrivateBase * pPrimitive=NULL);
    CObjectBase(const CObjectBase& other);
    virtual ~CObjectBase();

public:
    virtual pump_bool_t IsNull() const;
    virtual void SetNull(pump_bool_t bIsNull);
    pump_bool_t IsHeap() const;
    pump_bool_t operator==(const CObjectBase& other);

public:
    void* operator new(size_t size);
    void* operator new(size_t size, PUMP_CORE_ALLOC_TYPE emType);
    void operator delete(void* ptr);
    void operator delete(void* ptr, PUMP_CORE_ALLOC_TYPE emType);
    PUMP_CORE_ALLOC_TYPE GetAllocType() const;
    size_t GetSize() const;
private:
    PUMP_CORE_ALLOC_TYPE m_emType;
    size_t m_size;
protected:
    pump_bool_t m_bNull;
    const  char *m_szName;
    CObjectPrivateBase * m_pPrimitive; ///< pointer to primitive, but must not free. [Not Owner]. The owner is derived class.
};

class PUMP_CORE_CLASS CNullObject
    : virtual public CObjectBase
{
public:
    CNullObject();
};

class PUMP_CORE_CLASS CObjectMgrBase
{
public:
    CObjectMgrBase();
    virtual ~CObjectMgrBase();
};

}
}

#endif //PUMP_CORE_OBJCET_H