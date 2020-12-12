//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_FDEVLISTENER_H
#define PUMP_EVENT_FDEVLISTENER_H
#include "pump_event/pump_event_config.h"
#include "pump_event/event_listener.h"
#include "pump_event/handle_event/handle_event_container.h"
#include "pump_core/network/pump_core_multiplex.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

class PUMP_EVENT_CLASS CHandleEventListener
    : public CEventListener
{
public:
    /**
    * 获取CHandleEventListener单例指针，调用时创建
    * @param [in] refEvCollector 事件收集器，用于注入监听器的事件容器
    * @param [in] pParam 初始化参数，决定创建IO复用后端
    * @param [in] dwSize 参数数据长度
    */
    static CEventListener * getInstance(
        PUMP_ARG_IN CEventCollector &refEvCollector,
        PUMP_ARG_IN void *pParam = NULL,
        PUMP_ARG_IN size_t dwSize = 0);

    /**
     * @brief 初始化FDEvListener的参数对象
     */
    struct HandleEventListenerArg
    {
        /** 创建IO多路复用后端类型 */
        ::Pump::Core::Net::PUMP_BACKEND_TYPE emBackendType_;
    };
public:
    virtual ~CHandleEventListener();

    virtual int route(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0,
        PUMP_ARG_OUT void *pArgOut = NULL,
        PUMP_ARG_OUT size_t *dwSizeOut = NULL);

    ::Pump::Core::Net::CMultiplexBackend * get_backend();

#ifdef _TEST_LEVEL_DEBUG

    CMultidimEventContainer * get_fd_hash_container() { return m_pEvContainer; }

#endif //_TEST_LEVEL_DEBUG

protected:
    CHandleEventListener();

    CHandleEventListener(const CHandleEventListener &);

    virtual int
        init(PUMP_ARG_IN CEventCollector &refEvCollector,
        PUMP_ARG_IN void *pParam,
        PUMP_ARG_IN size_t dwSize);

    virtual int preListen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0);

    virtual int listen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0);

    virtual int postListen(PUMP_ARG_IN void *pArgIn = NULL,
        PUMP_ARG_IN size_t dwSizeIn = 0);

private:
    /**
    * 文件句柄IO多路复用后端，<所有者>负责构造和销毁
    */
    ::Pump::Core::Net::CMultiplexBackend * m_pBackend; //! <所有者>负责构造和销毁
    /**
    * 事件容器，创建事件监听器时，把容器注入到事件收集器CEventCollector
    * <所有者>允许销毁, 创建后需要向 EventCollector 注册
    */
    CMultidimEventContainer * m_pEvContainer;

protected:
    /**
    * 句柄事件监听器(单例)
    */
    static CHandleEventListener *s_pEvListener;
};

}
}
}

#endif // PUMP_EVENT_FDEVLISTENER_H
