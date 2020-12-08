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
        // �Ѵ���FDEvListener����
        return CHandleEventListener::s_pEvListener;
    }
    CHandleEventListener *pListener = new CHandleEventListener();
    if (PUMP_OK != pListener->init(refEvCollector, pParam, dwSize))
    {
        // TODO: ���Ӵ�����
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
        // TODO: ��������
        return PUMP_ERROR;
    }
    CHandleEventListener::HandleEventListenerArg *arg = (CHandleEventListener::HandleEventListenerArg *) pParam;
    // 1.��ʼ��FdEvent����
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
        // TODO: ��ʼ��FdEvent����ʧ��
        return PUMP_ERROR;
    }
    // 2.��ʼ��fd��·���ú��
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
            // TODO: ��������
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
    // FIXME ����ǰ���¼��б�
    //  m_pPreEvents->runAll();
    return 0;
}

int CHandleEventListener::listen(PUMP_ARG_IN void *pArgIn,
                             PUMP_ARG_IN size_t dwSizeIn)
{
#ifdef _TEST_LEVEL_INFO
    PUMP_CORE_INFO << "fd_event_listener::listen() in";
#endif // _TEST_LEVEL_INFO
    // 1.����::Pump::Core::Net::CFdRetList����,���ڻ�ȡ�����¼���fd
    ::Pump::Core::Net::CFdRetList fdRetList;
#ifdef _TEST_LEVEL_DEBUG
    // FIXME ��ʱʱ��Ŀǰ����һ���̶���ʱ��
    timeval tmOut;
    tmOut.tv_sec = 3;
    tmOut.tv_usec = 0;
#endif //_TEST_LEVEL_DEBUG
    // 2.ʹ�ö�·���ö����ȡ�����¼���fd
    int ret = m_pBackend->wait(fdRetList, tmOut);
    if (ret == -1)
    {
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "__CMultiplexBackend::wait() fail";
#endif // _TEST_LEVEL_INFO
    } 
    else 
    {
        // TODO ��ӷ��ش���
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "__CMultiplexBackend::wait() succ fd_event(" << ret << ")";
#endif // _TEST_LEVEL_INFO
        
        // 3.���������¼���fd, �������Ӧ���¼�����,�������¼�����Ͷ�ݵ���Ӧ���¼���������
        for (::Pump::Core::Net::CFdRetList::iterator it = fdRetList.begin();
             it != fdRetList.end(); ++it)
        {
            if (!m_pEvContainer)
            {
                // m_evContainer���ͷ�,˵����������
                throw 0;
            }
            
            // 3.1 ��fd���������¼������в��ҷ����¼���fd,���FileDescriptor����
            ::Pump::Event::HandleEvent::FindCond findCond;
            findCond.dwSize_ = sizeof(findCond);
            findCond.fd_ = (*it).fd_;
            
            CHandleCap *pFD = dynamic_cast<CHandleCap *>(
                m_pEvContainer->find(&findCond));
            if (pFD == NULL || pFD->m_pHandle->GetHandle() != (*it).fd_)
            {
                /* TODO: δ�ҵ�fd �� fd��ƥ�� */
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO << "error: δ�ҵ�fd �� fd��ƥ��";
#endif // _TEST_LEVEL_INFO
                continue;
            }
            // 3.2 ����fd�¼���״̬,����FileDescriptor�������¼�
            switch ((*it).re_fd_ev_)
            {
                case ::Pump::Core::Net::PUMP_IOEV_IN: // 3.2.1 fd�ɶ�
                {
                    switch (pFD->m_emState)
                    {
                        case FD_STATE_SOCK_LISTENED: // ��fd���ڼ���״̬ʱ,ִ��accept()����
                        {
                            //����accept�¼�
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_SOCK_LISTENED";
#endif // _TEST_LEVEL_DEBUG
                            CHandleCap::FindByFdEventCond FCond;
                            FCond.fdEvType_ = FDEV_TYPE_SOCK_ACCEPT;
                            FCond.dwSize_ = sizeof(FCond);
                            CEvent *pEv = pFD->find(&FCond);
                            if (pEv == NULL)
                            {
                                //TODO: err ��δ�ҵ�SockAccept�¼�, ˵��������
                                continue;
                            }
                            // FIXME �˴���Ӧ�ò���һ�������¼�Ͷ�ݵ�ActivedEventMgr?
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // �����¼�,�ͷ���Դ
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        case FD_STATE_SOCK_CONNECTING: // ��fd������������ʱ,˵��TCP���ӳɹ�
                        {
#ifdef _TEST_LEVEL_DEBUG
                            PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_IN::FD_STATE_SOCK_CONNECTING";
#endif // _TEST_LEVEL_DEBUG
                        } break;
                        case FD_STATE_SOCK_CONNECTED: //��fd��������״̬,˵����Ϣ����
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
                                //TODO: err ��δ�ҵ�SockRecv�¼�, ˵��������
                                continue;
                            }
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // �����¼�,�ͷ���Դ
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        case FD_STATE_TIMER_OPEN: // ��ʱ�����ڿ���״̬
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
                                //TODO: err ��δ�ҵ�SockRecv�¼�, ˵��������
                                continue;
                            }
                            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
                            if (pEv->m_emEvPeriod == CEvent::ONETIME)
                            {
                                // �����¼�,�ͷ���Դ
                                pFD->remove(pEv);
                                pEv->_on_destroyed(*m_pEvEngine, CEvent::EvData());
                                delete pEv;
                            }
                        } break;
                        default:
                            break;
                    }
                } break;
                case ::Pump::Core::Net::PUMP_IOEV_OUT: // 3.2.2 fd��д
                {
                    //TODO: ����һ�������¼�Ͷ�ݵ�EventReactor
#ifdef _TEST_LEVEL_INFO
                    PUMP_CORE_INFO << "::Pump::Core::Net::PUMP_IOEV_OUT::";
#endif // _TEST_LEVEL_INFO
                } break;
                case ::Pump::Core::Net::PUMP_IOEV_ERR: // 3.2.2 fd���ֹ���
                {
                    //TODO: ����һ�������¼�Ͷ�ݵ�EventReactor
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
    // FIXME ���к����¼��б�
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