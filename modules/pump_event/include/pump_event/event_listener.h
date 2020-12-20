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
 * @brief 事件观察者对象
 *
 * 监听条件事件
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
    CEventEngine *m_pEvEngine; //!< 绑定事件引擎(非所有者，禁止释放)，构造时绑定
    CEventCollector *m_pEvCollector; //!< 绑定事件收集器(非所有者，禁止释放)，构造时绑定
    const int m_iId; //!<未使用
    void *m_pParam;//!<未使用
    const char *m_szUuid; ///< 监听器uuid用于和CEvent配对
};

typedef CEventListener *LPEventListener;

}
}

#endif // PUMP_EVENT_EVENTLISTENER_H
