//
// Created by yz on 18-10-21.
//

#ifndef PUMP_MEMORY_POLICY_HPP
#define PUMP_MEMORY_POLICY_HPP
#include "pump_macro/pump_pre.h"
#include <utility>
#include <memory>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/thread/pump_core_atomic_op.hpp"
#include "pump_function/pump_function.hpp"
#include "pump_memory/pump_memory_def.h"
#include "pump_memory/smart_ptr/block.hpp"

namespace Pump {
namespace Memory {

//#ifdef WIN32
//inline int atomic_exchange_and_add(int * pw, int dv)
//{
//    nsp_b_detail::spinlock_pool<1>::scoped_lock lock(pw);
//
//    int r = *pw;
//    *pw += dv;
//    return r;
//}
//
//inline void atomic_increment(int * pw)
//{
//    nsp_b_detail::spinlock_pool<1>::scoped_lock lock(pw);
//    ++*pw;
//}
//
//inline int atomic_conditional_increment(int * pw)
//{
//    nsp_b_detail::spinlock_pool<1>::scoped_lock lock(pw);
//
//    int rv = *pw;
//    if (rv != 0)++*pw;
//    return rv;
//}
//#endif //WIN32

/**
* @class Policy
* @brief �й��ڴ�Ĺ���(�ͷ�)���Զ������
*  \aע�������������������Ϊ�Ѷ���ÿһ��ʵ��������Ψһ���ұض���һ��Block���� \
*       Policy��Block������Ȩ��
*/
class Policy
    : public Pump::Core::CNonCopyable {
public:
    /**
    * @var m_px
    * @brief �ڴ�������ĵ�ַ \
    *    \a������������ֻ����ֵ���ͷ�
    */
    Block *m_px;

    explicit Policy(Block const *px) : m_px(const_cast<Block*>(px)) {}

    virtual ~Policy() {}

    /**
    * @fn destroy
    * @brief ��������
    */
    virtual void destroy() = 0; // nothrow
    /**
    * @fn release
    * @brief �ͷ�Block*�ķ���
    */
    virtual void release() = 0; // nothrow

private:
    Policy();
};

template<typename _Ax = std::allocator<char> >
class SHeapPolicy
  : public Policy {
private:
  
  SHeapPolicy(SHeapPolicy const &);
  
  SHeapPolicy &operator=(SHeapPolicy const &);
  
  explicit SHeapPolicy(Block *px) : Policy(px) {}

  int use_count_;        // #shared
  Pump::Core::Thread::CAtomicOpGuilder<int> ao_use_count_;
  int weak_count_;       // #weak + (#shared != 0)
  Pump::Core::Thread::CAtomicOpGuilder<int> ao_weak_count_;

public:
  typedef typename _Ax::template rebind<char>::other _Alty;
  typedef Heap <_Ax> type_block;
  typedef PUMP_GFN(void,(void *)) func_t;
  
  /**
  * @var fn_destruct
  * @brief �û������ڴ�󣬿��ܻ���ڸõ�ַ�ڴ��е��ù��캯��
  */
  func_t fn_destruct;
  
  explicit SHeapPolicy(type_block *px)
    : Policy(static_cast<Block *>(px)),
      use_count_(1),
      ao_use_count_(use_count_),
      weak_count_(1),
      ao_weak_count_(weak_count_),
      fn_destruct() {
  }
  
  virtual ~SHeapPolicy() // nothrow
  {
  }
  
  // dispose() is called when use_count_ drops to zero, to release
  // the resources managed by *this.
  
  virtual void dispose() = 0; // nothrow
  
  // destroy() is called when weak_count_ drops to zero.
  
  virtual void destroy() // nothrow
  {
    delete this;
  }
  
  virtual void *get_deleter(/*FIXED �ĳ��Լ��ĵ�SP_TypeInfo���� boost::detail::sp_typeinfo*/int  const &ti) = 0;
  
  virtual void *get_untyped_deleter() = 0;
  
  void add_ref_copy() {
//#ifdef WIN32
//      atomic_increment(&use_count_);
//#elif (defined __linux__)
//      nsp_b_detail::atomic_increment(&use_count_);
//#endif // WIN32
      ++ao_use_count_;
  }
  
  bool add_ref_lock() // true on success
  {
//#ifdef WIN32
//      return atomic_conditional_increment(&use_count_) != 0;
//#elif (defined __linux__)
//      return nsp_b_detail::atomic_conditional_increment(&use_count_) != 0;
//#endif // WIN32
      ++ao_use_count_;
      return true;
  }
  
  virtual void release() // nothrow
  {
      //#ifdef WIN32
      //    if (atomic_exchange_and_add(&use_count_, -1) == 1) {
      //#elif (defined __linux__)
      //    if (nsp_b_detail::atomic_exchange_and_add(&use_count_, -1) == 1) {
      //#endif // WIN32

      /*FIXME �˴��ڶ��߳�����£����ܳ��־�̬������
       * ����--ao_use_count_ �� ao_use_count_.Value() ֮������������̲߳��� ao_use_count_*/
      if ((--ao_weak_count_).Value() == 1)
      {
          dispose();
          weak_release();
      }
  }
  
  void weak_add_ref() // nothrow
  {
//#ifdef WIN32
//    atomic_increment(&weak_count_);
//#elif (defined __linux__)
//    nsp_b_detail::atomic_increment(&weak_count_);
//#endif // WIN32
    ++ao_use_count_;
  }
  
  void weak_release() // nothrow
  {
//#ifdef WIN32
//      if (atomic_exchange_and_add(&weak_count_, -1) == 1) {
//#elif (defined __linux__)
//      if (nsp_b_detail::atomic_exchange_and_add(&weak_count_, -1) == 1) {
//#endif // WIN32
//          destroy();
//      }

      /*FIXME �˴��ڶ��߳�����£����ܳ��־�̬������
      * ����--ao_use_count_ �� ao_use_count_.Value() ֮������������̲߳��� ao_use_count_*/
      if ((--ao_weak_count_).Value() == 1)
      {
          destroy();
      }
  }
  
  long use_count() const // nothrow
  {
    return static_cast<int const volatile &>( use_count_ );
  }
};

template<class _Ax, class _D>
class SHeapPolicyImpl_ad
  : public SHeapPolicy<_Ax> {
private:
  _Ax a_; // copy constructor must not throw
  _D d_; // copy constructor must not throw
  
  SHeapPolicyImpl_ad(SHeapPolicyImpl_ad const &);
  
  SHeapPolicyImpl_ad &operator=(SHeapPolicyImpl_ad const &);
  
  typedef SHeapPolicyImpl_ad<_Ax, _D> this_type;
  typedef typename SHeapPolicy<_Ax>::type_block type_block;

public:
  
  // pre: d( p ) must not throw
  
  SHeapPolicyImpl_ad(type_block *p, _Ax &a, _D d)
    : SHeapPolicy<_Ax>(p), a_(a), d_(d) {
  }
  
  SHeapPolicyImpl_ad(type_block *p, size_t iSize, _D d)
    : SHeapPolicy<_Ax>(p), a_(), d_(d) {
  }

  // �����й��ڴ����
  virtual void dispose() // nothrow
  {
      // 1)������������
      if (!this->fn_destruct.Empty())
          this->fn_destruct(this->m_px);
      // 2)����ɾ�����黹�й�����
      d_(this->m_px);
      // 3)�����ڴ�������
      typedef typename std::allocator_traits<_Ax>::template rebind_alloc<type_block> A2;
      A2 a2(a_);
      a2.deallocate((typename A2::pointer) this->m_px, 1);
      this->m_px = 0;
  }

  // ���ٱ�����
  virtual void destroy() // nothrow
  {
      /** [ASK] �������������, ������ʱ�����������������, ��֪��sp_counted_base::weak_release()������ \
      * �������Ӧ������������ SHeapPolicyImpl_ad ��������, ����Ϊʲôʹ��std::allocator����, �����ʱ�����, ��������Ϊ�� \
      * ��ɾ����, ��Ĭ��Ϊ SHeapPolicyImpl_ad �������ɷ����������, ��������ʱ�Զ��黹������
      *
      * [ANSWER]  allocator_traits::rebind_alloc����Ϊallocator�趨���䵥λ
      */
      typedef typename std::allocator_traits<_Ax>::template rebind_alloc<this_type> A2;
      //
      A2 a2(a_);
      std::allocator_traits<A2>::destroy(a2, this); // ���ñ�������������
      a2.deallocate((typename A2::pointer) this, 1); // �黹�ڴ��������
  }
  
  virtual void *get_deleter(/*FIXED �ĳ��Լ��ĵ�SP_TypeInfo���� boost::detail::sp_typeinfo*/int const &ti) {
//    return ti == BOOST_SP_TYPEID( _D )? &reinterpret_cast<char&>( d_ ): 0;
    return 0;
  }
  
  virtual void *get_untyped_deleter() {
//    return &reinterpret_cast<char&>( d_ );
    return 0;
  }
};

template<class _Ax>
class SHeapPolicyImpl_a
  : public SHeapPolicy<_Ax> {
private:
  _Ax a_; // copy constructor must not throw
  
  SHeapPolicyImpl_a(SHeapPolicyImpl_a const &);
  
  SHeapPolicyImpl_a &operator=(SHeapPolicyImpl_a const &);
  
  typedef SHeapPolicyImpl_a<_Ax> this_type;
  typedef typename SHeapPolicy<_Ax>::type_block type_block;

public:
  
  // pre: d(p) must not throw
  
  SHeapPolicyImpl_a(type_block *p, _Ax &a)
    : SHeapPolicy<_Ax>(p), a_(a) {
  }
  
  SHeapPolicyImpl_a(type_block *p)
    : SHeapPolicy<_Ax>(p), a_() {
  }

  virtual void dispose() // nothrow
  {
      // 1)������������
      if (!this->fn_destruct.Empty())
          this->fn_destruct(this->m_px);
      // 2)�����й��ڴ�
      this->m_px->deallocate();
      // 3)�����ڴ�������
      typedef typename std::allocator_traits<_Ax>::template rebind_alloc<type_block> A2;
      A2 a2(a_);
      a2.deallocate((typename A2::pointer) this->m_px, 1);
      this->m_px = 0;
  }

  virtual void destroy() // nothrow
  {
      /** FIXME [urgency] �������������, ������ʱ�����������������, ��֪��sp_counted_base::weak_release()������ \
      * �������Ӧ������������ SHeapPolicyImpl_ad ��������, ����Ϊʲôʹ��std::allocator����, �����ʱ�����, ��������Ϊ�� \
      * ��ɾ����, ��Ĭ��Ϊ SHeapPolicyImpl_ad �������ɷ����������, ��������ʱ�Զ��黹������
      */
      typedef typename std::allocator_traits<_Ax>::template rebind_alloc<this_type> A2;

      A2 a2(a_);
      std::allocator_traits<A2>::destroy(a2, this);
      a2.deallocate((typename A2::pointer) this, 1);
  }
  
  virtual void *get_deleter(/*FIXED �ĳ��Լ��ĵ�SP_TypeInfo���� boost::detail::sp_typeinfo*/int  const &ti) {
    return 0;
  }
  
  virtual void *get_untyped_deleter() {
    return 0;
  }
};

/**
* @class PolicyGuider
* @brief Policy������󣬹�����BlockGuider���ơ�ͬ������������Policy�ڴ�Ĳ���
*/
class PolicyGuider {
protected:
  Policy *m_pp;
public:
  PolicyGuider() : m_pp(0) {}
  
  explicit PolicyGuider(Policy const * pp) : m_pp(const_cast<Policy*>(pp)) {}
  
  explicit PolicyGuider(const PolicyGuider &r) : m_pp(r.m_pp) {}
  
  virtual ~PolicyGuider() {}
  
  bool empty() const {
    return (m_pp == 0);
  }
  
  PolicyGuider &operator=(const PolicyGuider &r) {
    m_pp = (r.m_pp);
    return *this;
  }
  
  void swap(PolicyGuider &r) {
    std::swap(m_pp, r.m_pp);
  }
  
};

template <typename _Ax>
class WHeapPolicyGuider;

template<typename _Ax>
class SHeapPolicyGuider
  : public PolicyGuider {
private:
  typedef SHeapPolicy<_Ax> type_policy_base;
  typedef typename type_policy_base::type_block type_block;
  typedef typename _Ax::template rebind<char>::other _Alty;
#define p_policy(var) static_cast<type_policy_base*>(var)
  
  friend class WHeapPolicyGuider<_Ax>;

protected:
  _Alty a_;
public:
  SHeapPolicyGuider() : PolicyGuider() {};
  
 explicit SHeapPolicyGuider(type_block *p, _Ax &a)
      : PolicyGuider()/*,
                      pAdj_(0)*/,
                      a_(a) {
      try {
          m_pp = (SHeapPolicyImpl_a<_Ax> *) a_.allocate(sizeof(SHeapPolicyImpl_a<_Ax>));
          a_.construct(m_pp, p, a);
      }
      catch (...) {
          // ���ü���������ʧ��, ����Ҫɾ������
          a_.deallocate((typename _Alty::pointer) p, sizeof(SHeapPolicyImpl_a<_Ax>));
          throw;
      }
      //    try {
      //      pAdj_ = (AdjacencyNode *) alloc.allocate(sizeof(AdjacencyNode_a<_Ax>));
      //      ::new(pAdj_)AdjacencyNode_a<_Ax>(alloc);
      //    }
      //    catch (...) {
      //      // ���ü���������ʧ��, ����Ҫɾ������
      //      alloc.deallocate((typename _Ax::pointer) m_pp, sizeof(SHeapPolicyImpl_a<_Ax>));
      //      alloc.deallocate((typename _Ax::pointer) p, iSize);
      //      throw;
      //    }
  }

 explicit SHeapPolicyGuider(HeapGuider <_Ax> &p)
     : PolicyGuider()/*,
                     pAdj_(0)*/,
                     a_(p.get_alloc()) {
     try {
         m_pp = (SHeapPolicyImpl_a<_Ax> *) a_.allocate(sizeof(SHeapPolicyImpl_a<_Ax>));
         //      a_.construct(m_pp, p.get_block_priv());
         ::new(m_pp)SHeapPolicyImpl_a<_Ax>(p.get_block_priv());
     }
     catch (...) {
         // ���ü���������ʧ��, ����Ҫɾ������
         a_.deallocate((typename _Alty::pointer) p.get_block_priv(), sizeof(SHeapPolicyImpl_a<_Ax>));
         throw;
     }
     //    try {
     //      pAdj_ = (AdjacencyNode *) alloc.allocate(sizeof(AdjacencyNode_a<_Ax>));
     //      ::new(pAdj_)AdjacencyNode_a<_Ax>(alloc);
     //    }
     //    catch (...) {
     //      // ���ü���������ʧ��, ����Ҫɾ������
     //      alloc.deallocate((typename _Ax::pointer) m_pp, sizeof(SHeapPolicyImpl_a<_Ax>));
     //      alloc.deallocate((typename _Ax::pointer) p, iSize);
     //      throw;
     //    }
 }

 template<class D>
 SHeapPolicyGuider(type_block *p, _Ax &a, D d)
     : PolicyGuider()/*,
                     pAdj_(0)*/,
                     a_(a) {
     try {
         m_pp = (SHeapPolicyImpl_ad<_Ax, D> *) a_.allocate(sizeof(SHeapPolicyImpl_ad<_Ax, D>));
         //      a_.construct(m_pp, p, a, d);
         ::new(m_pp)SHeapPolicyImpl_ad<_Ax, D>(p.get_block_priv(), a, d);
         // FIXME [general] Ӧ�ø�Ϊʹ�÷���������,����SpCountedImpl_ad����sp_counted_base������, ��Ҫ����
         //      m_pp = new SHeapPolicyImpl_ad<_Ax, D>(p,iSize, alloc, d);
     }
     catch (...) {
         d(p, a); // delete p
         throw;
     }
     //    try {
     //      pAdj_ = (AdjacencyNode *) alloc.allocate(sizeof(AdjacencyNode_a<_Ax>));
     //      ::new(pAdj_)AdjacencyNode_a<_Ax>(alloc);
     //    }
     //    catch (...) {
     //      // ���ü���������ʧ��, ����Ҫɾ������
     //      alloc.deallocate((typename _Ax::pointer) m_pp, sizeof(SHeapPolicyImpl_ad<_Ax, D>));
     //      alloc.deallocate((typename _Ax::pointer) p, iSize);
     //      throw;
     //    }
 }
  
 template<class D>
 SHeapPolicyGuider(HeapGuider <_Ax> &p, D d)
     : PolicyGuider()/*,
                     pAdj_(0)*/,
                     a_(p.get_alloc()) {
     try {
         m_pp = (SHeapPolicyImpl_ad<_Ax, D> *) a_.allocate(sizeof(SHeapPolicyImpl_ad<_Ax, D>));
         //      a_.construct(m_pp, p.get_block_priv(), a, d);
         ::new(m_pp)SHeapPolicyImpl_ad<_Ax, D>(p.get_block_priv(), p.get_alloc(), d);
     }
     catch (...) {
         d(p.get_block_priv()); // delete p
         throw;
     }
     //    try {
     //      pAdj_ = (AdjacencyNode *) alloc.allocate(sizeof(AdjacencyNode_a<_Ax>));
     //      ::new(pAdj_)AdjacencyNode_a<_Ax>(alloc);
     //    }
     //    catch (...) {
     //      // ���ü���������ʧ��, ����Ҫɾ������
     //      alloc.deallocate((typename _Ax::pointer) m_pp, sizeof(SHeapPolicyImpl_ad<_Ax, D>));
     //      alloc.deallocate((typename _Ax::pointer) p, iSize);
     //      throw;
     //    }
 }

 virtual ~SHeapPolicyGuider() // nothrow
 {
     if (m_pp != 0) p_policy(m_pp)->release();
     // FIXME [urgent] Ŀǰ�ڽӵ����bug���ᵼ�±�������ʱ��֪��ԭ���������´���
     //    if (pAdj_ != 0 && m_pp != 0 && m_pp->use_count() == 0) pAdj_->release();
 }
  
  SHeapPolicyGuider(SHeapPolicyGuider const &r) : PolicyGuider(r)/*, pAdj_(r.pAdj_)*/ // nothrow
  {
    if (m_pp != 0) p_policy(m_pp)->add_ref_copy();
  }
  
  SHeapPolicyGuider(SHeapPolicyGuider &&r) : PolicyGuider(r)/*, pAdj_(r.pAdj_)*/ // nothrow
  {
    r.m_pp = 0;
  }
  
  explicit SHeapPolicyGuider(WHeapPolicyGuider<_Ax> const &r); // throws bad_weak_ptr when r.use_count() == 0
  SHeapPolicyGuider(WHeapPolicyGuider<_Ax> const &r, const NullPtr & ref); // constructs an empty *this when r.use_count() == 0

  SHeapPolicyGuider &operator=(SHeapPolicyGuider const &r) // nothrow
  {
    type_policy_base *tmp = p_policy(r.m_pp);
    bool bDel = false;
    if (tmp != m_pp) {
      if (tmp != 0) tmp->add_ref_copy();
      if (m_pp != 0) {
        if (p_policy(m_pp)->use_count() == 1) {
          bDel = true;
        }
        p_policy(m_pp)->release();
      }
      m_pp = tmp;
    }

//    AdjacencyNode *tmp2 = r.pAdj_;
//
//    if (tmp2 != pAdj_) {
//      if (pAdj_ != 0 && bDel) pAdj_->release();
//      pAdj_ = tmp2;
//    }
    
    return *this;
  }

//  void addIndegree(const void *in) {
//    pAdj_->addIndegree(in);
//  }
//
//  void addOutdegree(const void *out) {
//    pAdj_->addOutdegree(out);
//  }
  
  void swap(SHeapPolicyGuider &r) // nothrow
  {
    PolicyGuider::swap(r);
    std::swap(a_, r.a_);

//    AdjacencyNode *tmp2 = r.pAdj_;
//    r.pAdj_ = pAdj_;
//    pAdj_ = tmp2;
  }
  
  long use_count() const // nothrow
  {
    return (m_pp != 0 ? p_policy(m_pp)->use_count() : 0);
  }
  
  bool unique() const // nothrow
  {
    return (use_count() == 1);
  }
  
  friend inline bool operator==(SHeapPolicyGuider const &a, SHeapPolicyGuider const &b) {
    return (a.m_pp == b.m_pp);
  }
  
  friend inline bool operator<(SHeapPolicyGuider const &a, SHeapPolicyGuider const &b) {
    return std::less<SHeapPolicyGuider *>()(a.m_pp, b.m_pp);
  }

//  void * get_deleter( sp_typeinfo const & ti ) const
//  {
//    return m_pp? m_pp->get_deleter( ti ): 0;
//  }
  
  void *get_untyped_deleter() const {
    return (m_pp ? p_policy(m_pp)->get_untyped_deleter() : 0);
  }

public:
  typename type_policy_base::func_t &fn_destruct() { return p_policy(m_pp)->fn_destruct; }

#undef p_policy
};

template<typename _Ax = std::allocator<char> >
class WHeapPolicyGuider
  : public PolicyGuider {
private:
  typedef SHeapPolicy<_Ax> type_policy_base;
  typedef typename type_policy_base::type_block type_block;
  typedef typename _Ax::template rebind<char>::other _Alty;
#define p_policy(var) static_cast<type_policy_base*>(var)
  
  friend class SHeapPolicyGuider<_Ax>;

public:
  
  WHeapPolicyGuider() : PolicyGuider() {}
  
  WHeapPolicyGuider(SHeapPolicy<_Ax> const &r)
    : PolicyGuider(r.m_pp) // nothrow
  {
    if (m_pp != 0) p_policy(m_pp)->weak_add_ref();
  }
  
  WHeapPolicyGuider(WHeapPolicyGuider const &r)
    : PolicyGuider(r.m_pp) // nothrow
  {
    if (m_pp != 0) p_policy(m_pp)->weak_add_ref();
  }
  
  WHeapPolicyGuider(SHeapPolicyGuider<_Ax> const &r)
    : PolicyGuider(r.m_pp) // nothrow
  {
    if (m_pp != 0) p_policy(m_pp)->weak_add_ref();
  }

// Move support

//#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )
//
//  WHeapPolicyGuider(WHeapPolicyGuider &&r) : m_pp(r.m_pp) // nothrow
//  {
//    r.pi_ = 0;
//  }
//
//#endif
  
  ~WHeapPolicyGuider() // nothrow
  {
    if (m_pp != 0) p_policy(m_pp)->weak_release();
  }

  WHeapPolicyGuider &operator=(WHeapPolicyGuider const &r) // nothrow
  {
    type_policy_base *tmp = p_policy(r.m_pp);
    
    if (tmp != m_pp) {
      if (tmp != 0) p_policy(tmp)->weak_add_ref();
      if (m_pp != 0) p_policy(m_pp)->weak_release();
      m_pp = tmp;
    }
    
    return *this;
  }
  
  void swap(WHeapPolicyGuider &r) // nothrow
  {
    std::swap(r.m_pp, m_pp);
  }
  
  long use_count() const // nothrow
  {
    return (m_pp != 0 ? p_policy(m_pp)->use_count() : 0);
  }
  
  bool empty() const // nothrow
  {
    return (m_pp == 0);
  }
  
  friend inline bool operator==(WHeapPolicyGuider const &a, WHeapPolicyGuider const &b) {
    return (a.m_pp == b.m_pp);
  }
  
  friend inline bool operator<(WHeapPolicyGuider const &a, WHeapPolicyGuider const &b) {
    return std::less<type_policy_base*>()(a.m_pp, b.m_pp);
  }
};

template <typename _Ax>
inline SHeapPolicyGuider<_Ax>::SHeapPolicyGuider(WHeapPolicyGuider<_Ax> const &r)
  : PolicyGuider(r.m_pp)
{
  if (m_pp == 0 || !p_policy(m_pp)->add_ref_lock()) {
     // TODO ���ɱ�׼��
      throw "bad_weak_ptr";
  }
}

template <typename _Ax>
inline SHeapPolicyGuider<_Ax>::SHeapPolicyGuider(WHeapPolicyGuider<_Ax> const & r, const NullPtr & ref)
  : PolicyGuider( r.m_pp )
{
  if( m_pp != 0 && !p_policy(m_pp)->add_ref_lock() )
  {
    m_pp = 0;
//    pAdj_ = 0;
  }
}

}
}

#endif //PUMP_MEMORY_POLICY_HPP
