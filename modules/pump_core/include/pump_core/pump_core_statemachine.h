/**
 * @file pumnp_core_statemachine.h
 * @brief Library Header.
 * @details Declaration of state machine interface.
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

#ifndef PUMP_EVENT_STATEMACHINE_H
#define PUMP_EVENT_STATEMACHINE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/thread/pump_core_mutex.h"

namespace Pump
{
namespace Core
{

#define PUMP_CORE_STATE_DEFAULT (-1)

class PUMP_CORE_CLASS CStateMachineBase
{
public:
    CStateMachineBase();

    virtual ~CStateMachineBase();

    bool Cheak() const;

    void ReSetState();

    void SetState(unsigned int dwState);

    unsigned int GetStateLocked();

    void Unlock();

    unsigned int GetState() const;

private:
    unsigned int m_dwState;
protected:
    Pump::Core::Thread::CMutex  m_csThis;
};

}
}

#endif // PUMP_EVENT_STATEMACHINE_H