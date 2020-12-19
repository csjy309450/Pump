//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENTLISTENER_H
#define PUMP_EVENT_EVENTLISTENER_H

#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/actived_event_mgr.h"

namespace Pump
{
namespace Event
{

class CEventListenerMgr;

/**
 * @class EventListener []
 * @brief �¼��۲��߶���
 *
 * ���������¼�
 */
PUMP_ABSTRACT
class PUMP_EVENT_CLASS CEventListener
    : public ::Pump::Core::CNonCopyOperator
{
    friend CEventListenerMgr;
public:
    virtual ~CEventListener();

    virtual int route(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0,
        PUMP_ARG_OUT void *pArgOut = NULL,
        PUMP_ARG_OUT size_t *dwSizeOut = NULL);

#ifdef _TEST_LEVEL_DEBUG

    CEventCollector *get_event_collector() { return m_pEvCollector; }

#endif //_TEST_LEVEL_DEBUG

protected:
    CEventListener();

    explicit CEventListener(const char *);

    virtual int init(PUMP_ARG_IN CEventCollector &refEvCollector,
        PUMP_ARG_IN void *pParam,
        PUMP_ARG_IN size_t dwSize);

    virtual int preListen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0) = 0;

    virtual int listen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0) = 0;

    virtual int postListen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0) = 0;

protected:
    CEventEngine *m_pEvEngine; //!< ���¼�����(�������ߣ���ֹ�ͷ�)������ʱ��
    CEventCollector *m_pEvCollector; //!< ���¼��ռ���(�������ߣ���ֹ�ͷ�)������ʱ��
    const int m_iId; //!<δʹ��
    void *m_pParam;//!<δʹ��
    const char *m_szUuid; ///< ������uuid���ں�CEvent���
};

typedef CEventListener *LPEventListener;

}
}

#endif // PUMP_EVENT_EVENTLISTENER_H
