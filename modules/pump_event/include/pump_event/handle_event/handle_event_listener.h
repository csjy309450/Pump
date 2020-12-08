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
    * ��ȡCHandleEventListener����ָ�룬����ʱ����
    * @param [in] refEvCollector �¼��ռ���������ע����������¼�����
    * @param [in] pParam ��ʼ����������������IO���ú��
    * @param [in] dwSize �������ݳ���
    */
    static CEventListener * getInstance(
        PUMP_ARG_IN CEventCollector &refEvCollector,
        PUMP_ARG_IN void *pParam = NULL,
        PUMP_ARG_IN size_t dwSize = 0);

    /**
     * @brief ��ʼ��FDEvListener�Ĳ�������
     */
    struct HandleEventListenerArg
    {
        /** ����IO��·���ú������ */
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
    * �ļ����IO��·���ú�ˣ�<������>�����������
    */
    ::Pump::Core::Net::CMultiplexBackend * m_pBackend; //! <������>�����������
    /**
    * �¼������������¼�������ʱ��������ע�뵽�¼��ռ���CEventCollector
    * <������>��������, ��������Ҫ�� EventCollector ע��
    */
    CMultidimEventContainer * m_pEvContainer;

protected:
    /**
    * ����¼�������(����)
    */
    static CHandleEventListener *s_pEvListener;
};

}
}
}

#endif // PUMP_EVENT_FDEVLISTENER_H
