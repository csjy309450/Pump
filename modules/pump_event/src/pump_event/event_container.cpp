//
// Created by yz on 19-5-26.
//

#include "pump_event/event_container.h"
#include <cstddef>
#include <cassert>

namespace Pump
{
namespace Event
{

CEvent *CUnidimEventContainer::begin() { return NULL; }

CEvent *CUnidimEventContainer::end() { return NULL; }

const CEvent *CUnidimEventContainer::cbegin() const { return NULL; }

const CEvent *CUnidimEventContainer::cend() const { return NULL; }

bool CUnidimEventContainer::empty() const { return false; }

size_t CUnidimEventContainer::size() const { return 0; }

bool CUnidimEventContainer::anchor(CEvent *) { return true; }

CEvent *CUnidimEventContainer::get() { return NULL; }

CEvent *CUnidimEventContainer::next() { return NULL; }

const CEvent *CUnidimEventContainer::cnext() { return NULL; }

CEvent *CUnidimEventContainer::prior() { return NULL; }

const CEvent *CUnidimEventContainer::cprior() { return NULL; }

bool CUnidimEventContainer::insert(CEvent *pEv, const Condition *) { return false; }

bool CUnidimEventContainer::remove(CEvent *pEv, const Condition *) { return false; }

CEvent *CUnidimEventContainer::find(const Condition *) { return NULL; }

bool CUnidimEventContainer::clear() { return false; }

CUnidimEventContainer *CMultidimEventContainer::begin() { return NULL; }

CUnidimEventContainer *CMultidimEventContainer::end() { return NULL; }

const CUnidimEventContainer *CMultidimEventContainer::cbegin() const { return NULL; }

const CUnidimEventContainer *CMultidimEventContainer::cend() const { return NULL; }

bool CMultidimEventContainer::empty() const { return false; }

size_t CMultidimEventContainer::size() const { return 0; }

CUnidimEventContainer *CMultidimEventContainer::get() { return NULL; }

bool CMultidimEventContainer::anchor(CUnidimEventContainer *) { return true; }

bool CMultidimEventContainer::insert(CUnidimEventContainer *pEvContainer, const Condition *) { return false; }

bool CMultidimEventContainer::insert(CEvent *pEv, const Condition *) { return false; }

bool CMultidimEventContainer::remove(CUnidimEventContainer *pEvContainer, const Condition *) { return false; }

bool CMultidimEventContainer::remove(CEvent *pEv, const Condition *) { return false; }

CUnidimEventContainer *CMultidimEventContainer::next() { return NULL; }

const CUnidimEventContainer *CMultidimEventContainer::cnext() { return NULL; }

CUnidimEventContainer *CMultidimEventContainer::prior() { return NULL; }

const CUnidimEventContainer *CMultidimEventContainer::cprior() { return NULL; }

CUnidimEventContainer *CMultidimEventContainer::find(const Condition *) { return NULL; }

bool CMultidimEventContainer::clear() { return false; }

CEventContainer::CEventContainer() : m_pEvConatiner(NULL) {}

CEventContainer::~CEventContainer()
{
    if (this->m_pEvConatiner)
    {
        switch (this->m_emContainerType)
        {
        case EventContainerType::Multidim:
            delete (CMultidimEventContainer *)m_pEvConatiner;
            break;
        case EventContainerType::Unidim:
            delete (CUnidimEventContainer *)m_pEvConatiner;
            break;
        default:
            break;
        }
    }
}

}
}