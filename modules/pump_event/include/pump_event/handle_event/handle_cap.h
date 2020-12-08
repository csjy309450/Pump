//
// Created by yz on 19-6-28.
//
#ifndef PUMP_EVENT_FILEDESCRIPTOR_H
#define PUMP_EVENT_FILEDESCRIPTOR_H
#include <map>
#include "pump_core/pump_core_handle.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/event.h"
#include "pump_event/handle_event/handle_event.h"
#include "pump_event/event_container.h"

//using namespace ::std;

namespace Pump
{
namespace Event
{
namespace HandleEvent
{
class CHandleEventHashContainer;

/**
 * @brief FileDescriptor ��ע�������FdEvent��������,ֱ���û�������ֹ�¼�ǰ,
 * fd�¼����������ڹ�FdEventList����
 */
class PUMP_EVENT_CLASS  CHandleCap
    : public CUnidimEventContainer
{
    friend CHandleEventHashContainer;
public:
    struct FindByFdEventCond
        : public Condition
    {
        FdEventType fdEvType_;
    };

public:
    typedef std::pair<FdEventType, CHandleEvent *> ItemType;
    typedef std::map<FdEventType, CHandleEvent *> ContainerType;
    typedef ContainerType::iterator IteratorType;

    /**
     * @var unsigned short fd_ev_
     * @brief io�ļ��������ļ����¼�, ��ȡ(�ǻ���):
     * - ::Pump::Core::Net::PUMP_IOEV_IN
     * - IO_EV_OUT
     * - IO_EV_ERR
     */
    unsigned short m_fdEv;
    //pump_fd_t m_fd; //! �ļ�������
    ::Pump::Core::CHandleBase * m_pHandle;
    FdState m_emState; //! FdEvent����������״̬
    ContainerType m_mapEv; //! FdEvent�����������,�ƿ�FdEvent����

    CHandleCap();

    virtual ~CHandleCap();

    CHandleCap(const CHandleCap &other);

    CHandleCap &operator=(const CHandleCap &other);

#ifdef _TEST_LEVEL_DEBUG

    ContainerType &getMapEvent() { return this->m_mapEv; }

#endif //_TEST_LEVEL_DEBUG

    virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);

    virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);

    virtual CEvent *find(const Condition *pCond = NULL);

    virtual bool clear();

private:

    virtual CEvent *begin();

    virtual CEvent *end();

    virtual const CEvent *cbegin() const;

    virtual const CEvent *cend() const;

    virtual bool anchor(CEvent *);

    virtual CEvent *get();

    virtual CEvent *next();

    virtual const CEvent *cnext();

    virtual CEvent *prior();

    virtual const CEvent *cprior();
};

}
}
}

#endif //PUMP_EVENT_FILEDESCRIPTOR_H
