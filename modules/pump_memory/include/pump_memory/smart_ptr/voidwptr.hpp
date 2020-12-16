//
// Created by yz on 18-10-23.
//
#ifndef PUMP_VOIDWPTR_HPP
#define PUMP_VOIDWPTR_HPP

#include <utility>
#include <cstring>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "pump_core/pump_core_logger.h"
#include "pump_memory/smart_ptr/block.hpp"
#include "pump_memory/smart_ptr/policy.hpp"

namespace nsp_std = ::std;

namespace Pump {
namespace Memory {

class VoidSPtr;

bool operator==(VoidSPtr const &p, const NullPtr&);
bool operator==(const NullPtr&, VoidSPtr const &p);
bool operator!=(VoidSPtr const &p, const NullPtr&);
bool operator!=(const NullPtr&, VoidSPtr const &p);
bool operator==(VoidSPtr const &a, VoidSPtr const &b);
bool operator!=(VoidSPtr const &a, VoidSPtr const &b);
std::ostream &operator<<(std::ostream &os, VoidSPtr const &p);

class VoidWPtr {
protected:
  
  // Borland 5.5.1 specific workarounds
  typedef VoidWPtr this_type;

public:
  
  VoidWPtr()  // never throws in 1.30+
    : m_block(),
      m_policy() {
  }

//  generated copy constructor, assignment, destructor are fine...

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

// ... except in C++0x, move disables the implicit copy
  
  VoidWPtr(VoidWPtr const &r)
    : m_block(r.m_block),
      m_policy(r.m_policy) {
  }
  
  VoidWPtr &operator=(VoidWPtr &r)  {
    m_block = r.m_block;
    m_policy = r.m_policy;
    return *this;
  }

#endif //!defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

//
//  The "obvious" converting constructor implementation:
//
//  template<class Y>
//  VoidWPtr(VoidWPtr<Y> const & r): m_px(r.m_px), m_policy(r.m_policy) // never throws
//  {
//  }
//
//  has a serious problem.
//
//  r.m_px may already have been invalidated. The m_px(r.m_px)
//  conversion may require access to *r.m_px (virtual inheritance).
//
//  It is not possible to avoid spurious access violations since
//  in multithreaded programs r.m_px may be invalidated at any point.
//

//  template<class Y>
//#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )
//
//  VoidWPtr( VoidWPtr<Y> const & r, typename boost::detail::sp_enable_if_convertible<Y,T>::type = boost::detail::sp_empty() )
//
//#else
//
//  VoidWPtr( VoidWPtr<Y> const & r )
//
//#endif
//   : m_px(r.lock().get()), m_policy(r.m_policy)
//  {
//    boost::detail::sp_assert_convertible< Y, T >();
//  }

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

//  template<class Y>
//#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )
//
//  VoidWPtr( VoidWPtr<Y> && r, typename boost::detail::sp_enable_if_convertible<Y,T>::type = boost::detail::sp_empty() )
//
//#else
//
//  VoidWPtr( VoidWPtr<Y> && r )
//
//#endif
//   : m_px( r.lock().get() ), m_policy( static_cast< WeakCount && >( r.m_policy ) )
//  {
//    boost::detail::sp_assert_convertible< Y, T >();
//    r.m_px = 0;
//  }
  
  // for better efficiency in the T == Y case
  VoidWPtr(VoidWPtr &&r)
   : m_block(r.m_block), m_policy(static_cast< WHeapPolicyGuider<> && >( r.m_policy )) {
    r.m_block = HeapGuider<>();
  }
  
  // for better efficiency in the T == Y case
  VoidWPtr &operator=(VoidWPtr &&r)  {
    this_type(static_cast< VoidWPtr && >( r )).swap(*this);
    return *this;
  }
  
#endif //!defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

  VoidWPtr(VoidSPtr const &r) ;
  /*  : m_block(r.m_block),
      m_policy(r.m_policy) {
  }*/
  
  VoidWPtr & operator=(VoidSPtr const &r) ;/* {
    m_block = r.m_block;
    m_policy = r.m_policy;
    return *this;
  }*/

//  template<class Y>
//#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )
//
//  VoidWPtr( SharedPtr<Y> const & r, typename boost::detail::sp_enable_if_convertible<Y,T>::type = boost::detail::sp_empty() )
//
//#else
//
//  VoidWPtr( SharedPtr<Y> const & r )
//
//#endif
//   : m_px( r.m_px ), m_policy( r.m_policy )
//  {
//    boost::detail::sp_assert_convertible< Y, T >();
//  }
  
  VoidSPtr lock_raw() const ; /*{
    return VoidSPtr(*this, boost::detail::sp_nothrow_tag());
  }*/
  
  long use_count() const  {
    return m_policy.use_count();
  }
  
  bool expired() const  {
    return m_policy.use_count() == 0;
  }
  
  bool _empty() const // extension, not in std::VoidWPtr
  {
    return m_policy.empty();
  }
  
  void reset()  // never throws in 1.30+
  {
    this_type().swap(*this);
  }
  
  void swap(this_type &other)  {
    m_block.swap(other.m_block);
    m_policy.swap(other.m_policy);
  }

//  template<class Y> bool owner_before( VoidWPtr<Y> const & rhs ) const
//  {
//    return m_policy < rhs.m_policy;
//  }
//
//  template<class Y> bool owner_before( SharedPtr<Y> const & rhs ) const
//  {
//    return m_policy < rhs.m_policy;
//  }

// Tasteless as this may seem, making all members public allows member templates
// to work in the absence of member template friends. (Matthew Langston)

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

protected:
  
  friend class VoidSPtr;

#endif
  /**
   * @var m_block
   * @brief 托管内存对象
   */
  HeapGuider<> m_block;
  /**
 * @var m_policy
 * @brief 引用计数对象
 */
  WHeapPolicyGuider<> m_policy; // reference counter
};

}
}

#endif //PUMP_VOIDWPTR_HPP
