//
// Created by yz on 19-5-26.
//
#include <stdlib.h>
#include "pump_event/event_listener.h"

namespace Pump
{
namespace Event
{

CEventListener::CEventListener()
  : m_iId(),
    m_szUuid("")
{

}

CEventListener::CEventListener(const char *szUUID)
  : m_iId(),
    m_szUuid(szUUID)
{
  
}

CEventListener::~CEventListener()
{
}

int
CEventListener::init(PUMP_ARG_IN CEventCollector &refEvCollector,
                    PUMP_ARG_IN void *pParam,
                    PUMP_ARG_IN size_t dwSize)
{
  CEventCollector *pwork = &refEvCollector;
  m_pEvCollector = &refEvCollector;
  m_pEvEngine = &refEvCollector.get_event_engine();
  return 0;
}

/**
* 监听器例程
* @param pArgIn 例程入参
* @param dwSizeIn 入参数据长度
* @param pArgIn 例程出参
* @param dwSizeIn 出数据长度
*/
int CEventListener::route(PUMP_ARG_IN void *pArgIn,
                         PUMP_ARG_IN size_t dwSizeIn,
                         PUMP_ARG_OUT void *pArgOut,
                         PUMP_ARG_OUT size_t *dwSizeOut)
{
  this->preListen();
  this->listen();
  this->postListen();
  return 0;
}

}
}