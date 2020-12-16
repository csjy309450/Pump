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

namespace Pump
{
namespace Core
{

CObjectBase::CObjectBase()
    : m_bNull(PUMP_TRUE)
    , m_szName("Null")
    , m_pPrimitive(NULL)
{}

CObjectBase::CObjectBase(const char* szName, pump_bool_t bIsNull, CPrimitiveBase * pPrimitive)
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
    return malloc(size);
}

void CObjectBase::operator delete(void* ptr)
{
    free(ptr);
}

//CNonCopyOperator::CNonCopyOperator(const char* szName, pump_bool_t bIsNull, CPrimitiveBase * pPrimitive)
//    : CObjectBase(szName, bIsNull, pPrimitive)
//{}

CNonCopyOperator::CNonCopyOperator() {}

CNonCopyOperator::~CNonCopyOperator()
{}

CNullObject::CNullObject()
    : CObjectBase()
{}

//CNonNewOperator::CNonNewOperator(const char* szName, pump_bool_t bIsNull, CPrimitiveBase * pPrimitive)
//    : CObjectBase(szName, bIsNull, pPrimitive)
//{}

CNonNewOperator::CNonNewOperator()
{}

CNonNewOperator::~CNonNewOperator()
{}

}
}