/**
 * @file pumnp_core_statemachine.cpp
 * @brief Library Implementation.
 * @details Implementation of state machine interface.
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

#include "pump_core\pumnp_core_statemachine.h"

namespace Pump
{
namespace Core
{

CStateMachineBase::CStateMachineBase()
    :m_dwState(PUMP_CORE_STATE_DEFAULT)
{

}

CStateMachineBase::~CStateMachineBase()
{

}

bool CStateMachineBase::Cheak() const
{
    return (m_dwState == -1);
}

void CStateMachineBase::ReSetState()
{
    Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    m_dwState = -1;
}

void CStateMachineBase::SetState(unsigned int dwState)
{
    Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    m_dwState = dwState;
}

unsigned int CStateMachineBase::GetStateLocked()
{
    m_csThis.Lock();
    return m_dwState;
}

void CStateMachineBase::Unlock()
{
    m_csThis.Unlock();
}

unsigned int CStateMachineBase::GetState() const
{
    return m_dwState;
}

}
}
