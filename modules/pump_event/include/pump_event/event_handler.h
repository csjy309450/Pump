#ifndef PUMP_EVENT_EVENT_HANDLER
#define PUMP_EVENT_EVENT_HANDLER
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_nonop.h"
#include "pump_function/pump_function.hpp"
#include "pump_event/pump_event_config.h"

namespace Pump
{
namespace Event
{

class CEventLoop;

class PUMP_EVENT_CLASS CEventHandler
    : public ::Pump::Core::CNonCopyOperator
{
public:
    CEventHandler();
    CEventHandler(void* pData);
    virtual ~CEventHandler();

    void SetData(void* pData) { m_pData = pData; }
    void SetEventLoop(CEventLoop * pLoop) { m_pLoop = pLoop; }
    CEventLoop * GetEventLoop() { return m_pLoop; }
    virtual pump_int32_t Open(pump_pvoid_t) { return 0; }
    virtual pump_int32_t Close(pump_pvoid_t) { return 0; }
protected:
    void* m_pData;
    CEventLoop * m_pLoop;
};

}
}

#endif // PUMP_EVENT_EVENT_HANDLER
