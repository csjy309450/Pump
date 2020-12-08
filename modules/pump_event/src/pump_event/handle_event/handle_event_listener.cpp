//
// Created by yz on 19-5-26.
//
#include <cstdio>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_event/handle_event/handle_event_container.h"
#include "pump_event/handle_event/handle_event_listener.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

CHandleEventListener *CHandleEventListener::s_pEvListener = NULL;

CEventListener *CHandleEventListener::getInstance(PUMP_ARG_IN CEventCollector &refEvCollector,
                                              PUMP_ARG_IN void *pParam,
                                              PUMP_ARG_IN size_t dwSize)
{
    if (CHandleEventListener::s_pEvListener != NULL)
    {
        // 已存在FDEvListener单例
        return CHandleEventListener::s_pEvListener;
    }
    CHandleEventListener *pListener = new CHandleEventListener();
    if (PUMP_OK != pListener->init(refEvCollector, pParam, dwSize))
    {
        // TODO: 增加错误码
        delete pListener;
        return NULL;
    }
    CHandleEventListener::s_pEvListener = pListener;
    return pListener;
}

CHandleEventListener::~CHandleEventListener()
{
    if (m_pBackend)
    {
        delete m_pBackend;
    }
    
    if (m_pEvContainer)
    {
        delete m_pEvContainer;
    }
}

int
CHandleEventListener::init(PUMP_ARG_IN CEventCollector &refEvCollector,
                       PUMP_ARG_IN void *pParam,
                       PUMP_ARG_IN size_t dwSize)
{
    CEventCollector *pwork = &refEvCollector;
    CEventListener::init(refEvCollector, pParam, dwSize);
    if (pParam == NULL || sizeof(CHandleEventListener::HandleEventListenerArg) != dwSize)
    {
        // TODO: 参数错误
        return PUMP_ERROR;
    }
    CHandleEventListener::HandleEventListenerArg *arg = (CHandleEventListener::HandleEventListenerArg *) pParam;
    // 1.初始化FdEvent容器
#ifdef PUMP_OS_WINDOWS
    m_pEvContainer = new CHandleEventMapContainer();
#elif (defined PUMP_OS_POSIX)
    m_pEvContainer = new CHandleEventHashContainer();
#endif // PUMP_OS_WINDOWS
    if (!m_pEvCollector->get_listened_event_mgr().insert_event_container(
        CEventContainer::Multidim,
        (pump_handle_t) m_pEvContainer,
        get_uuid()))
    {
        // TODO: 初始化FdEvent容器失败
        return PUMP_ERROR;
    }
    // 2.初始化fd多路复用后端
    switch (arg->emBackendType_)
    {
        case ::Pump::Core::Net::PUMP_SELECT:
            m_pBackend = new ::Pump::Core::Net::CSelect();
            break;
        case ::Pump::Core::Net::PUMP_EPOLL:
            //m_pBackend = new ::Pump::Core::Net::CEpoll();
            break;
        case ::Pump::Core::Net::PUMP_POLL:
            //m_pBackend = new ::Pump::Core::Net::CPoll();
            break;
        default:
            // TODO: 参数错误
            return PUMP_ERROR;
    }
    return PUMP_OK;
}

int CHandleEventListener::route(PUMP_ARG_IN void *pArgIn,
                            PUMP_ARG_IN size_t dwSizeIn,
                            PUMP_ARG_OUT void *pArgOut,
                            PUMP_ARG_OUT size_t *dwSizeOut)
{
    return CEventListener::route(pArgIn, dwSizeIn, pArgOut, dwSizeOut);
}

CHandleEventListener::CHandleEventListener()
    : CEventListener(get_uuid())
{

}

CHandleEventListener::CHandleEventListener(const CHandleEventListener &)
{

}

int CHandleEventListener::preListen(PUMP_ARG_IN void *pArgIn,
                                PUMP_ARG_IN size_t dwSizeIn)
{
    // FIXME 运行前置事件列表
    //  m_pPreEvents->runAll();
    return 0;
}

int CHandleEventListener::listen(PUMP_ARG_IN void *pArgIn,
                             PUMP_ARG_IN size_t dwSizeIn)
{
#ifdef _TEST_LEVEL_INFO
    PUMP_CORE_INFO << "fd_event_listener::listen() in";
#endif // _TEST_LEVEL_INFO
    // 1.构造::Pump::Core::Net::CFdRetList对象,用于获取发生事件的fd
    ::Pump::Core::Net::CFdRetList fdRetList;
#ifdef _TEST_LEVEL_DEBUG
    // FIXME 超时时间目前给了一个固定的时间
    timeval tmOut;
    tmOut.tv_sec = 3;
    tmOut.tv_usec = 0;
#endif //_TEST_LEVEL_DEBUG
    // 2.使用多路复用对象获取发生事件的fd
    int ret = m_pBackend->wait(fdRetList, tmOut);
    if (ret == -1)
    {
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "__CMultiplexBackend::wait() fail";
#endif // _TEST_LEVEL_INFO
    } 
    else 
    {
        // TODO 添加返回处理
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "__CMultiplexBackend::wait() succ fd_event(" << ret << ")";
#endif // _TEST_LEVEL_INFO
        
        // 3.遍历发生事件的fd, 激活其对应的事件对象,并根据事件类型投递到对应的事件管理容器
        for (::Pump::Core::Net::CFdRetList::iterator it = fdRetList.begin();
             it != fdRetList.end(); ++it)
        {
            if (!m_pEvContainer)
            {
                // m_evContainer被释放,说明出问题了
                throw 0;
            }
            
            // 3.1 在fd监听器的事件容器中查找发生事件的fd,获得FileDescriptor对象
            ::Pump::Event::HandleEvent::FindCond findCond;
            findCond.dwSize_ = sizeof(findCond);
            findCond.fd_ = (*it).fd_;
            
            CHandleCap *pFD = dynamic_cast<CHandleCap *>(
                m_pEvContainer->find(&findCond));
            if (pFD == NULL || pFD->m_pHandle->GetHandle() != (*it).fd_)
            {
                /* TODO: 未找到fd 或 fd不匹配 */
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO << "error: 未找到fd 或 fd不匹配";
#endif // _TEST_LEVEL_INFO
                continue;
            }
            // 3.2 根据fd事件的状态,激活FileDescriptor关联的事件
            switch ((*it).re_fd_ev_)
            {
                case ::Pump::Core::Net::PUMP_IOEV_IN: // 3.2.1 fd可读
                {
                    switch (pFD->m_emState)
                    {
                        case FD_STATE_SOCK_LISTENED: // 当fd处于监听状态时,执行accept()操作
                        {
                            //处理accept事件
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_SOCK_LISTENED";
#endif // _TEST_LEVEL_DEBUG
                            CHandleCap::FindByFdEventCond FCond;
                            FCond.fdEvType_ = FDEV_TYPE_SOCK_ACCEPT;
                            FCond.dwSize_ = sizeof(FCond);
                            CEvent *pEv = pFD->find(&FCond);
                            if (pEv == NULL)
                            {
                                //TODO: err 若未找到SockAccept事件, 说明出错了
                                continue;
                            }
                            // FIXME 此处是应该产生一个立即事件投递到ActivedEventMgr?
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // 销毁事件,释放资源
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        case FD_STATE_SOCK_CONNECTING: // 当fd处于正在连接时,说明TCP连接成功
                        {
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_SOCK_CONNECTING";
#endif // _TEST_LEVEL_DEBUG
                        } break;
                        case FD_STATE_SOCK_CONNECTED: //当fd处于链接状态,说明消息到达
                        {
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_SOCK_CONNECTED";
#endif // _TEST_LEVEL_DEBUG
                            CHandleCap::FindByFdEventCond FCond;
                            FCond.fdEvType_ = FDEV_TYPE_SOCK_RECV;
                            FCond.dwSize_ = sizeof(FCond);
                            CEvent *pEv = pFD->find(&FCond);
                            if (pEv == NULL)
                            {
                                //TODO: err 若未找到SockRecv事件, 说明出错了
                                continue;
                            }
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // 销毁事件,释放资源
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        case FD_STATE_TIMER_OPEN: // 定时器处于开启状态
                        {
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_TIMER_OPEN";
#endif // _TEST_LEVEL_DEBUG
                            CHandleCap::FindByFdEventCond FCond;
                            FCond.fdEvType_ = FDEV_TYPE_TIMER_CREART;
                            FCond.dwSize_ = sizeof(FCond);
                            CEvent *pEv = pFD->find(&FCond);
                            if (pEv == NULL)
                            {
                                //TODO: err 若未找到SockRecv事件, 说明出错了
                                continue;
                            }
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // 销毁事件,释放资源
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        default:
                            break;
                    }
                } break;
                case ::Pump::Core::Net::PUMP_IOEV_OUT: // 3.2.2 fd可写
                {
                    //TODO: 产生一个立即事件投递到EventReactor
#ifdef _TEST_LEVEL_INFO
                    PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_OUT::";
#endif // _TEST_LEVEL_INFO
                } break;
                case ::Pump::Core::Net::PUMP_IOEV_ERR: // 3.2.2 fd出现故障
                {
                    //TODO: 产生一个立即事件投递到EventReactor
#ifdef _TEST_LEVEL_INFO
                    PUMP_CORE_INFO << "IO_EV_ERR::";
#endif // _TEST_LEVEL_INFO
                } break;
                default:
                    break;
            }
        }
    }
    return ret;
}

int CHandleEventListener::postListen(PUMP_ARG_IN void *pArgIn,
                                 PUMP_ARG_IN size_t dwSizeIn)
{
    // FIXME 运行后置事件列表
#ifdef _TEST_LEVEL_INFO
    PUMP_CORE_INFO << "fd_event_listener::postListen()";
#endif // _TEST_LEVEL_INFO
    //  m_pPostEvents->runAll();
    return 0;
}

::Pump::Core::Net::CMultiplexBackend * CHandleEventListener::get_backend()
{
    return m_pBackend;
}

}
}
}