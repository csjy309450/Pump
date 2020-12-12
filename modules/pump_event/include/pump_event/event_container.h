//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_EVENTCONTAINER_H
#define PUMP_EVENT_EVENTCONTAINER_H
#include <cstddef>
#include "pump_event/pump_event_config.h"
#include "pump_event/event.h"

namespace Pump
{
namespace Event
{

struct Condition
{
  size_t dwSize_;
};

/*
 * @brief 一维的事件容器的接口类
 */
PUMP_INTERFACE
class PUMP_EVENT_CLASS CUnidimEventContainer
{
public:
  
  friend class CListenedEventMgr;

protected:
  
  virtual CEvent *begin();
  
  virtual CEvent *end();
  
  virtual const CEvent *cbegin() const;
  
  virtual const CEvent *cend() const;
  
  virtual bool empty() const;
  
  virtual size_t size() const;
  
  virtual bool anchor(CEvent *);
  
  virtual CEvent *get();
  
  virtual CEvent *next();
  
  virtual const CEvent *cnext();
  
  virtual CEvent *prior();
  
  virtual const CEvent *cprior();
  
  virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);
  
  virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);
  
  virtual CEvent *find(const Condition *);
  
  virtual bool clear();
};

/*
* @brief 多维的事件容器的接口类
*/
PUMP_INTERFACE
class PUMP_EVENT_CLASS CMultidimEventContainer
{
public:
  
  friend class CListenedEventMgr;
  
  virtual CUnidimEventContainer *begin();
  
  virtual CUnidimEventContainer *end();
  
  virtual const CUnidimEventContainer *cbegin() const;
  
  virtual const CUnidimEventContainer *cend() const;
  
  virtual bool empty() const;
  
  virtual size_t size() const;
  
  virtual bool anchor(CUnidimEventContainer *);
  
  virtual CUnidimEventContainer *get();
  
  virtual CUnidimEventContainer *next();
  
  virtual const CUnidimEventContainer *cnext();
  
  virtual CUnidimEventContainer *prior();
  
  virtual const CUnidimEventContainer *cprior();
  
  virtual bool insert(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);
  
  virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);
  
  virtual bool remove(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);
  
  virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);
  
  virtual CUnidimEventContainer *find(const Condition *pCond = NULL);
  
  virtual bool clear();
};

/*
 * @brief 事件容器基类
 */
PUMP_ABSTRACT
class PUMP_EVENT_CLASS CEventContainer
{
public:
  enum EventContainerType
  {
    Multidim,
    Unidim
  };
  
  CEventContainer();
  
  virtual ~CEventContainer();
  
  void *m_pEvConatiner; //! <所有者>, 负责申请\销毁, 可以是一维也可以是多维
  EventContainerType m_emContainerType;
};

}
}

#endif //PUMP_EVENT_EVENTCONTAINER_H
