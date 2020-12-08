//
// Created by yz on 19-6-8.
//
#include "pump_macro/pump_pre.h"
#include <cerrno>
#include <sys/types.h>
#ifdef __linux__
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#elif (defined _WIN32)
#include <basetsd.h>
#include<ws2tcpip.h>
#include <winsock2.h>
typedef SSIZE_T ssize_t;
#endif // __linux__
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_engine.h"
#include "pump_core/network/pump_core_multiplex.h"
#include "pump_event/handle_event/socket_event.h"
#include "pump_event/handle_event/handle_event_container.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_event/event_interface.h"
#include "pump_event/event_loop.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

const int g_iIoBufferSize = 512;

CTCPSockAccept::CTCPSockAccept()
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_ACCEPT, EVPRIOR_DEFAULT, CONTINUOUS)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new CTCPSockAccept -> " << this;
#endif //_TEST_LEVEL_DEBUG
    memset(&m_struArg, 0, sizeof(m_struArg));
}

CTCPSockAccept::CTCPSockAccept(OnAcceptCbType & cb)
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_ACCEPT, EVPRIOR_DEFAULT, CONTINUOUS)
    , m_cbAccept(cb)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new CTCPSockAccept -> " << this;
#endif //_TEST_LEVEL_DEBUG
    memset(&m_struArg, 0, sizeof(m_struArg));
}

CTCPSockAccept::CTCPSockAccept(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_ACCEPT, emPrior, CONTINUOUS)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_accept -> " << this;
#endif //_TEST_LEVEL_DEBUG
    memset(&m_struArg, 0, sizeof(m_struArg));
}

CTCPSockAccept::~CTCPSockAccept()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "delete sock_accept -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

void CTCPSockAccept::Bind(OnAcceptCbType & cb)
{
    m_cbAccept = cb;
}

int CTCPSockAccept::_on_listend(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_listend() in";
#endif //_TEST_LEVEL_DEBUG
    ::Pump::Core::Net::CSock * pSock = dynamic_cast<::Pump::Core::Net::CSock *>(m_pHandle);
    if (!pSock)
    {
        return PUMP_ERROR;
    }
    ::Pump::Core::Net::CAddr objAddr(m_struArg.iAf_, m_struArg.szIp_, m_struArg.sPort);
    if (pSock->ListenExt(objAddr, m_struArg.iBackLog_) == -1)
    {
#ifdef _TEST_LEVEL_DEBUG
        PUMP_CORE_INFO << "listen() error: " << errno;
#endif //_TEST_LEVEL_DEBUG
        return -1;
    }
    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;
    int iSwitchAccept = 0;
    //socklen_t iSwitchAcceptLen;
    CHandleEventListener *pEvListener = NULL;
    ::Pump::Core::Net::CFdCtl change;

    // 1. ����ȡ fd_event_listener �󶨵��¼�����
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().
        find_event_container(this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�����
        goto ret_bad;
    }
    // 2 ����ȡ�ļ�����������FDescriptor
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_pHandle->GetHandle();
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor == NULL)
    {
        // TODO: err δ�ҵ��ļ�������
        goto ret_bad;
    }

    // ��ȡ��������
#ifdef __linux__
    if (::getsockopt(this->m_fd, SOL_SOCKET, SO_ACCEPTCONN, &iSwitchAccept, &iSwitchAcceptLen) == -1)
    {
        switch (errno)
        {
        case EBADF:
        case ENOTSOCK:
            //TODO: err �ļ��������д�
            goto ret_bad;
        default:
            //TODO: ��ȡ��������ʧ��
            break;
        }
    }
#endif // __linux__

    if (iSwitchAccept == 0)
    {
        //TODO : err ��������û����,��û�е���::listen()
    }

    // 3. ����ȡ fd_event_listener
    pEvListener = dynamic_cast<CHandleEventListener *>(refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));
    if (pEvListener == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�������
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        goto ret_bad;
    }
    // 4. ����Backend�¼�
    change.fd_ = this->m_pHandle->GetHandle();
    change.type_ = ::Pump::Core::Net::PUMP_FDCTL_ADD;
    change.fd_ev_ = ::Pump::Core::Net::PUMP_IOEV_IN;
    if (pEvListener->get_backend()->update(change) == -1)
    {
        //TODO: err ˢ��Backendʧ��
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        goto ret_bad;
    }
    // 5.�����ļ�����������״̬
    pFDescriptor->m_emState = FD_STATE_SOCK_LISTENED;
    pFDescriptor->m_fdEv = pFDescriptor->m_fdEv | ::Pump::Core::Net::PUMP_IOEV_IN;
    refEvEngine.unlock_listener_by_uuid(this->m_szUuid);

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_listend() out with 1";
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_listend() out with -1";
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CTCPSockAccept::_on_actived(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
    ::Pump::Core::Net::CSock * pSock = dynamic_cast<::Pump::Core::Net::CSock *>(m_pHandle);
    if (!pSock)
    {
        return PUMP_ERROR;
    }

    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;
    CHandleCap connectedSock;
    pump_fd_t fdConnector;
    //sockaddr_in addrConnector;
    const ::Pump::Core::Net::CAddr * pAddr = PUMP_NULL;
    //int flags;
    //socklen_t addrLen = sizeof(addrConnector);
    std::string strIp;
    pump_uint16_t sPort;
#ifdef __linux__
    pump_thread_id tid = pthread_self();
#endif //__linux__
    //CTCPSockRecv *pSockRecv = NULL;
    CHandleEventListener *pEvListener = NULL;
    ::Pump::Core::Net::CFdCtl change;
    // �������
    CTCPSockAccept::SockAcceptArg *pArg = NULL;
    if (objData.pData_ != NULL && objData.dwSize_ != sizeof(CTCPSockAccept::SockAcceptArg))
    {
        //TODO : err ��������
        goto ret_bad;
    }
    pArg = static_cast<CTCPSockAccept::SockAcceptArg *>(objData.pData_);

    // 1. ����ȡ fd_event_listener �󶨵ľ������
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�����
        goto ret_bad;
    }

ACCEPT:
    // 2. accept
    ::Pump::Core::Net::CSock * pClientSock = pSock->Accept();
    if (!pClientSock || !pClientSock->Available())
    {
        if (errno == EINTR)
        { // ϵͳ���ñ��жϣ���Ҫ���½���
            goto ACCEPT;
        }
        else
        {
#ifdef _TEST_LEVEL_ERROR
            PUMP_CORE_ERR << "accept fail, error(" << errno << "): " << strerror(errno);
#endif // _TEST_LEVEL_ERROR
            return -1;
        }
    }
    pAddr = pClientSock->GetAddr();
    pAddr->GetAddrStr(strIp);
    pAddr->GetPort(sPort);
#ifdef _TEST_LEVEL_INFO
    PUMP_CORE_INFO << "accept a new client: " << strIp.c_str() << ":" << sPort;
#endif // _TEST_LEVEL_INFO
    fdConnector = pClientSock->GetHandle();
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = fdConnector;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor != NULL && ((pFDescriptor->m_pHandle->Available())
        && (pFDescriptor->m_emState != FD_STATE_CLOSED
        && pFDescriptor->m_emState != FD_STATE_DEFAULT)))
    {
        // ���� pFDescriptor ������open״̬���ʾ�Ѿ���ʼ��
        //TODO: err:�ظ���ʼ��������
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " before insert";
#endif //_TEST_LEVEL_DEBUG

    if (pClientSock->SetNonBlock(PUMP_TRUE) != PUMP_OK)
    {
        switch (errno)
        {
        case EBADF:
            //TODO: err �ļ��������д�
            goto ret_bad;
        default:
            //TODO: ���÷�����ʧ��
            break;
        }
    }
    // ����ϵͳĬ�ϵĽ��ջ�������С
    if (pClientSock->SetBuffSize(g_iIoBufferSize, g_iIoBufferSize) != PUMP_OK)
    {
        switch (errno)
        {
        case EBADF:
        case ENOTSOCK:
            //TODO: err �ļ��������д�
            goto ret_bad;
        default:
            //TODO: ���û�����ʧ��
            break;
        }
    }
    // ��ʼ�� CFileDescriptor ����
    connectedSock.m_pHandle = pClientSock;
    connectedSock.m_emState = FD_STATE_SOCK_CONNECTED;
    connectedSock.m_mapEv.clear();

    // TODO �����û���Accept�ص���ʹ��CFunction
    m_cbAccept(pClientSock, &connectedSock, NULL);
    //// ������¼�,Ͷ�ݶ��¼�
    //pSockRecv = new CTCPSockRecv();
    //pSockRecv->m_fd = fdConnector;
    //pSockRecv->m_pHandle = pClientSock;
    //connectedSock.insert(pSockRecv);
    // 3.���� backend Io�¼�
    pEvListener = dynamic_cast<CHandleEventListener *>(
        refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));// ��ȡfd�¼�������
    change.fd_ = fdConnector;
    change.type_ = ::Pump::Core::Net::PUMP_FDCTL_ADD;
    change.fd_ev_ = ::Pump::Core::Net::PUMP_IOEV_IN;
    pEvListener->get_backend()->update(change);
    refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
    //����IOϵͳ�¼����
    connectedSock.m_fdEv = connectedSock.m_fdEv | ::Pump::Core::Net::PUMP_IOEV_IN;
    // connectSock ���뵽 pHashContainer
    pHashContainer->insert(&connectedSock);
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " after insert";
#endif //_TEST_LEVEL_DEBUG

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_actived() out with 1";
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_actived() out with -1";
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CTCPSockAccept::_on_destroyed(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_destroyed() in";
#endif //_TEST_LEVEL_DEBUG
    //FIXED ����
    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;
    // 1. ����ȡ fd_event_listener �󶨵��ļ��������
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�����
        goto ret_bad;
    }
    // 2 ����ȡ�ļ�����������FDescriptor
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor == NULL)
    {
        // TODO: err δ�ҵ��ļ�������
        goto ret_bad;
    }

    switch (pFDescriptor->m_emState)
    {
    case FD_STATE_SOCK_LISTENED:
    {
        CHandleEventListener *pEvListener = NULL;
        ::Pump::Core::Net::CFdCtl change;

        // 3. ����ȡ fd_event_listener
        pEvListener = dynamic_cast<CHandleEventListener *>(refEvEngine.get_listener_by_uuid_locked(
            this->m_szUuid));
        if (pEvListener == NULL)
        {
            // TODO: err δ�ҵ���Ӧ�¼�������
            refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
            goto ret_bad;
        }

        /*4.�Ӷ�·���ö���ɾ��fd*/
        change.fd_ = pFDescriptor->m_pHandle->GetHandle();
        change.type_ = ::Pump::Core::Net::PUMP_FDCTL_DEL;

        if (pEvListener->get_backend()->update(change) == -1)
        {
            /* FIXME ���ô����� */
            refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
            PUMP_CORE_INFO << "fd ���µ� MutiplexBackend ʧ��";
            goto ret_bad;
        }

        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        goto ret_good;
    }
    case FD_STATE_DEFAULT:
    case FD_STATE_SOCK_INITED:
    case FD_STATE_SOCK_CONNECTED:
    case FD_STATE_SOCK_CONNECTING:
    case FD_STATE_SOCK_SHUTDOWNED:
    case FD_STATE_CLOSED:
    default:
    {
        // TODO: ���ô�����
        PUMP_CORE_INFO << "fd ������������״̬������ disableAccept() ����";
        goto ret_bad;
    }
    }

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_destroyed() out with 1";
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_accept::_on_destroyed() out with -1";
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

CTCPSockConnect::CTCPSockConnect()
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_CONNECT, EVPRIOR_DEFAULT, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_connect -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockConnect::CTCPSockConnect(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_CONNECT, emPrior, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_connect -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockConnect::~CTCPSockConnect()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "delete sock_connect -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

int CTCPSockConnect::_on_actived(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_connect::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
    return 0;
}

CTCPSockRecv::CTCPSockRecv()
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_RECV, EVPRIOR_DEFAULT, CONTINUOUS)
    , m_objIoBufRecv(g_iIoBufferSize)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_recv -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockRecv::CTCPSockRecv(OnRecvCbType & cb)
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_RECV, EVPRIOR_DEFAULT, CONTINUOUS)
    , m_objIoBufRecv(g_iIoBufferSize)
    , m_cbRecv(cb)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_recv -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockRecv::CTCPSockRecv(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_RECV, emPrior, CONTINUOUS),
    m_objIoBufRecv(g_iIoBufferSize)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_recv -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockRecv::~CTCPSockRecv()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "delete sock_recv -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

void CTCPSockRecv::Bind(OnRecvCbType & cb)
{
    m_cbRecv = cb;
}

int CTCPSockRecv::_on_actived(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_recv::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
    ::Pump::Core::Net::CSock * pSock = dynamic_cast<::Pump::Core::Net::CSock *>(m_pHandle);
    if (!pSock)
    {
        return PUMP_ERROR;
    }
    // ��ʱ������
    static pump_char_t buf[g_iIoBufferSize];
    memset(buf, 0, sizeof(buf));
    ssize_t ret = -1;
    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;
    // 1. ����ȡ fd_event_listener �󶨵��¼�����
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�����
        goto ret_bad;
    }
    // 2 ����ȡ�ļ�����������FDescriptor
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(pHashContainer->find(&fCond));
    if (pFDescriptor == NULL)
    {
        // TODO: err δ�ҵ��ļ�������
        goto ret_bad;
    }
    // 3 recv data
    ret = pSock->Recv(buf, g_iIoBufferSize);
    if (ret == -1)
    {
        int iError = PUMP_CORE_GetSystemError();
        switch (iError)
        {
#ifdef PUMP_OS_WINDOWS
        case EWOULDBLOCK:
            // û������
            goto ret_good;
        case 10054:
        {
            this->__OnRemoteClose(pFDescriptor, refEvEngine);
        } break;
#endif // PUMP_OS_WINDOWS
        default:
            goto ret_bad;
        }
    }
    else if (ret == 0)
    {
        this->__OnRemoteClose(pFDescriptor, refEvEngine);
    }
    else
    {
#ifdef _TEST_LEVEL_DEBUG
        PUMP_CORE_INFO << "sock_recv::_on_actived() recv from :" << this->m_fd << "{\n" << buf << "\n}";
#endif //_TEST_LEVEL_DEBUG
        // ѹ����ջ�����
        this->m_objIoBufRecv.append(buf, static_cast<size_t>(ret));
        this->__OnRecv();
    }

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_recv::_on_actived() out with 1";
#endif //_TEST_LEVEL_DEBUG
    return PUMP_OK;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_recv::_on_actived() out with -1";
#endif //_TEST_LEVEL_DEBUG
    return PUMP_ERROR;
}

pump_int32_t CTCPSockRecv::__OnRemoteClose(CHandleCap * pFDescriptor, CEventEngine &refEvEngine)
{
#ifdef _TEST_LEVEL_INFO
    PUMP_CORE_INFO << "[warning] ���ӶϿ�";
#endif // _TEST_LEVEL_INFO
    // FIXME [critical] ȱ�����ӶϿ�����
    switch (pFDescriptor->m_emState)
    {
    case FD_STATE_SOCK_CONNECTED:
    {
        // ��ȡ fd_event_listener
        CHandleEventListener *pEvListener = dynamic_cast<CHandleEventListener *>(
            refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));
        if (pEvListener == NULL)
        {
            // TODO: err δ�ҵ���Ӧ�¼�������
            refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
            return PUMP_ERROR;
        }
        ::Pump::Core::Net::CFdCtl change;
        if ((pFDescriptor->m_fdEv & ::Pump::Core::Net::PUMP_IOEV_IN) != 0)
        {
            change.fd_ = this->m_fd;
            change.type_ = ::Pump::Core::Net::PUMP_FDCTL_MOD;
            change.fd_ev_ = pFDescriptor->m_fdEv & (~::Pump::Core::Net::PUMP_IOEV_IN);
        }
        else
        {
            // TODO: err
            refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
            break;
        }

        if (pEvListener->get_backend()->update(change) == -1)
        {
            /* FIXME ���ô����� */
            PUMP_CORE_INFO << "fd ���µ� MutiplexBackend ʧ��";
            refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
            return PUMP_ERROR;
        }
        /*����״̬*/
        pFDescriptor->m_fdEv = pFDescriptor->m_fdEv & (~::Pump::Core::Net::PUMP_IOEV_IN);
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
    }
    break;
    default:
    {
        // TODO: err ���ô�����
        PUMP_CORE_INFO << "fd ������������״̬������ disableRecv() ����";
        return PUMP_ERROR;
    }
    }
    return PUMP_OK;
}

pump_int32_t CTCPSockRecv::__OnRecv()
{
    m_cbRecv(this);
    return PUMP_OK;
}

CTCPSockSend::CTCPSockSend()
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_SEND, EVPRIOR_DEFAULT, ONETIME),
    m_objIoBufSend(g_iIoBufferSize)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_send -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockSend::CTCPSockSend(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_SOCK_SEND, emPrior, ONETIME),
    m_objIoBufSend(g_iIoBufferSize)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new sock_send -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

CTCPSockSend::~CTCPSockSend()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "delete sock_send -> " << this;
#endif //_TEST_LEVEL_DEBUG
}

int CTCPSockSend::_on_actived(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "sock_send::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
    return 0;
}

}
}
}