//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_ACTIVEDEVENTMGR_H
#define PUMP_EVENT_ACTIVEDEVENTMGR_H

#include <list>
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_memory/smart_buffer/circular_buffer_interface.hpp"
#include "pump_event/ev_def.h"
#include "pump_event/event.h"
#include "pump_event/event_container.h"

//using namespace Pump::Memory;
//class Pump::Core::CNonCopyOperator;
//class Pump::Event::CActivedEventPriorQueue;
//class Pump::Memory::circular_buffer_interface<Pump::Event::CActivedEventPriorQueue>;

namespace Pump
{
namespace Event
{

#define EVENT_PRIORQUEUE_LEN (Pump::Event::EventPriority::EVPRIOR_HIGHEST + 1) //!< ���ȼ��¼����еĳ���

/**
 * \brief �����¼�����, �����ȼ�����
 */
PUMP_IMPLEMENT
class CActivedEventPriorQueue
    : public ::Pump::Core::CNonCopyOperator,
    public Pump::Event::CMultidimEventContainer
{
public:
    class CActivedEventContainer
        : public ::Pump::Core::CNonCopyOperator,
        public Pump::Event::CUnidimEventContainer
    {
    public:
        typedef std::list<CEvent *> ContainerType;
        typedef ContainerType::iterator IteratorType;
    public:
        CActivedEventContainer();

        virtual CEvent *begin();

        virtual CEvent *end();

        virtual bool empty() const;

        virtual size_t size() const;

        virtual CEvent *get();

        virtual CEvent *next();

        virtual CEvent *prior();

        virtual bool clear();

        virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);

        virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);

    private:
        ContainerType m_objActiveEventContainer;
        IteratorType m_itWork;
    };

public:
    CActivedEventPriorQueue();

    virtual CActivedEventContainer *begin();

    virtual CActivedEventContainer *end();

    virtual CActivedEventContainer *get();

    virtual CActivedEventContainer *next();

    virtual CActivedEventContainer *prior();

    virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);

    /**
     * ɾ������pEv
     * @param [in] pEv �¼�ָ��
     * @param [in] pCond �Ƴ�����
     * @return
     */
    virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);

private:
    CActivedEventContainer m_arrPriorQueue[EVENT_PRIORQUEUE_LEN + 1];  //!< ���г�������1, �ڱ�
    CActivedEventContainer *m_pWork;
    // TODO: ���ӹ���̬�¼�����
};

class CActivedEventMgr
    : public ::Pump::Core::CNonCopyOperator
    , public ::Pump::Memory::circular_buffer_interface<CActivedEventPriorQueue>
{
public:
    CActivedEventMgr();

    virtual ~CActivedEventMgr();

    bool insert(CEvent *pEv);

    bool remove(CEvent *pEv);

    void lock_offscreen();

    void unlock_offscreen();

    virtual buf_iterator get_offscreen_unlocked();

    virtual void swap();

private:
    CActivedEventPriorQueue m_objActiveEventConatiner_0; //! Newly̬�¼�˫��������. ������Event������,�������ͷ�
    CActivedEventPriorQueue m_objActiveEventConatiner_1; //! Newly̬�¼�˫��������. ������Event������,�������ͷ�
    ::Pump::Core::Thread::CMutex m_csOffscreen; //! ���ڶ�Offscreen�������Ĳ�������
};

}
}

#endif // PUMP_EVENT_ACTIVEDEVENTMGR_H