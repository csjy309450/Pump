//
// Created by yz on 18-10-28.
//
#ifndef PUMP_WEAKPTR_HPP
#define PUMP_WEAKPTR_HPP
#include "pump_memory/pump_memory_def.h"
#include "voidsptr.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"
#include "pump_memory/smart_ptr/sharedptr.hpp"

namespace nsp_std = std;

namespace Pump {
namespace Memory {

template <class T>
class WeakPtr
    : public VoidWPtr {
protected:
    typedef WeakPtr<T> this_type;
public:

    typedef typename SP_Element<T>::ElmType element_type;

    WeakPtr()
        : VoidWPtr(),
        m_state(SP_STATE_NULL){
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "WeakPtr()";
#endif //_TEST_LEVEL_INFO
    }

    explicit WeakPtr(WeakPtr const &r)
        : VoidWPtr(static_cast<VoidWPtr const &>(r)),
        m_state(r.m_state){
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "WeakPtr()";
#endif //_TEST_LEVEL_INFO
    }

    template <class Y>
    explicit WeakPtr(SharedPtr<Y> const &r)
        : VoidWPtr(static_cast<VoidSPtr const &>(r)),
        m_state(r.m_state) {
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "WeakPtr()";
#endif //_TEST_LEVEL_INFO
    }

    virtual ~WeakPtr() {
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "~WeakPtr()";
#endif //_TEST_LEVEL_INFO
    }

    template<class Y>
    WeakPtr<T> &operator=(WeakPtr<Y> const &r) {
        VoidWPtr::operator=(static_cast<VoidWPtr const &>(r));
        m_state = (r.m_state);
        return *this;
    }

    template<class Y>
    WeakPtr<T> & operator=(SharedPtr<Y> const & r)
    {
        VoidWPtr::operator=(static_cast<VoidSPtr const &>(r));
        m_state = (r.m_state);
        return *this;
    }

        SharedPtr<T> lock() const
    {
        SharedPtr<T> sp = SharedPtr<T>(lock_raw());
        if (sp != PUMP_NULLPTR)
        {
            sp.m_state = m_state;
        }
        else {
            sp.m_state = SP_STATE_NULL;
        }
        return sp;
    }
protected:
    enum SpState m_state; //! 指针生命周期
};

}
}

#endif //PUMP_WEAKPTR_HPP
