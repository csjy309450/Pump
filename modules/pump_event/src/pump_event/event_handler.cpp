#include "pump_event/event_handler.h"

namespace Pump
{
namespace Event
{

CEventHandler::CEventHandler()
    : m_pData(NULL)
{
}

CEventHandler::CEventHandler(void* pData)
    : m_pData(pData)
{
}

CEventHandler::~CEventHandler()
{
}

}
}
