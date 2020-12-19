/**
 * @file pump_core_object.cpp
 * @brief Library Implementation.
 * @details Implementation of object system interface.
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

#include <cstddef>
#include <stdlib.h>
#include "pump_core/pump_core_object.h"
#include "pump_core/pump_core_allocator.h"

namespace Pump
{
namespace Core
{

CObjectPrivateBase::CObjectPrivateBase() 
{
}

CObjectPrivateBase::~CObjectPrivateBase() {}

PUMP_CORE_ALLOC_TYPE CObjectPrivateBase::GetAllocType() const
{
    return m_emType;
}

size_t CObjectPrivateBase::GetSize() const
{
    return m_size;
}

void* CObjectPrivateBase::operator new(size_t size)
{
    CObjectPrivateBase *ptr = (CObjectPrivateBase *)malloc(size);
    if (ptr)
    {
        *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_BUILDIN;
        *(size_t *)&(ptr->m_size) = size;
    }
    return ptr;
}

void* CObjectPrivateBase::operator new(size_t size, PUMP_CORE_ALLOC_TYPE emType)
{
    switch (emType)
    {
    case PUMP_CORE_ALLOC_MEMPOLL:
    {
        CObjectPrivateBase *ptr = (CObjectPrivateBase *)CAllocator::New(size);
        if (ptr)
        {
            *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_MEMPOLL;
            *(size_t *)&(ptr->m_size) = size;
        }
        return ptr;
    }
    case PUMP_CORE_ALLOC_BUILDIN:
    default:
    {
        CObjectPrivateBase *ptr = (CObjectPrivateBase *)malloc(size);
        if (ptr)
        {
            *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_BUILDIN;
            *(size_t *)&(ptr->m_size) = size;
        }
        return ptr;
    }
    }
    return NULL;
}

void CObjectPrivateBase::operator delete(void* ptr)
{
    if (ptr)
    {
        CObjectPrivateBase * pPirv = (CObjectPrivateBase *)ptr;
        switch (pPirv->GetAllocType())
        {
        case PUMP_CORE_ALLOC_MEMPOLL:
            CAllocator::Delete(ptr, pPirv->GetSize());
            break;
        case PUMP_CORE_ALLOC_BUILDIN:
        default:
        {
            free(ptr);
        }
        }
    }
}

CObjectBase::CObjectBase()
    : m_bNull(PUMP_TRUE)
    , m_szName("Null")
    , m_pPrimitive(NULL)
{}

CObjectBase::CObjectBase(const char* szName, pump_bool_t bIsNull, CObjectPrivateBase * pPrimitive)
    : m_bNull(bIsNull)
    , m_szName(szName) 
    , m_pPrimitive(pPrimitive)
{}

CObjectBase::CObjectBase(const CObjectBase& other)
    : m_bNull(other.m_bNull)
    , m_szName(other.m_szName)
    , m_pPrimitive(other.m_pPrimitive)
{

}

CObjectBase::~CObjectBase()
{
    m_bNull = (PUMP_TRUE);
}

pump_bool_t CObjectBase::IsNull() const
{
    return m_bNull;
}

void CObjectBase::SetNull(pump_bool_t bIsNull)
{
    m_bNull = bIsNull;
}

pump_bool_t CObjectBase::operator==(const CObjectBase& other)
{
    return (::strcmp(other.m_szName, m_szName) == 0);
}

void* CObjectBase::operator new(size_t size)
{
    CObjectBase *ptr = (CObjectBase *)malloc(size);
    if (ptr)
    {
        *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_BUILDIN;
        *(size_t *)&(ptr->m_size) = size;
    }
    return ptr;
}

void* CObjectBase::operator new(size_t size, PUMP_CORE_ALLOC_TYPE emType)
{
    switch (emType)
    {
    case PUMP_CORE_ALLOC_MEMPOLL:
    {
        CObjectBase *ptr = (CObjectBase *)CAllocator::New(size);
        if (ptr)
        {
            *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_MEMPOLL;
            *(size_t *)&(ptr->m_size) = size;
        }
        return ptr;
    }
    case PUMP_CORE_ALLOC_BUILDIN:
    default:
    {
        CObjectBase *ptr = (CObjectBase *)malloc(size);
        if (ptr)
        {
            *(PUMP_CORE_ALLOC_TYPE *)&(ptr->m_emType) = PUMP_CORE_ALLOC_BUILDIN;
            *(size_t *)&(ptr->m_size) = size;
        }
        return ptr;
    }
    }
    return NULL;
}

void CObjectBase::operator delete(void* ptr)
{
    if (ptr)
    {
        CObjectBase * pPirv = (CObjectBase *)ptr;
        switch (pPirv->GetAllocType())
        {
        case PUMP_CORE_ALLOC_MEMPOLL:
            CAllocator::Delete(ptr, pPirv->GetSize());
            break;
        case PUMP_CORE_ALLOC_BUILDIN:
        default:
        {
            free(ptr);
        }
        }
    }
}

PUMP_CORE_ALLOC_TYPE CObjectBase::GetAllocType() const
{
    return m_emType;
}

size_t CObjectBase::GetSize() const
{
    return m_size;
}

CNullObject::CNullObject()
    : CObjectBase()
{}

CObjectMgrBase::CObjectMgrBase() {}
CObjectMgrBase::~CObjectMgrBase() {}

}
}