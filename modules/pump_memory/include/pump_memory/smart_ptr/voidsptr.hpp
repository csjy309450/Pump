//
// Created by yz on 18-10-21.
//
#ifndef PUMP_VOIDSPTR2_HPP
#define PUMP_VOIDSPTR2_HPP
#include "pump_macro/pump_compiler_config.h"
#include <utility>
#include <cstring>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <sstream>
#if (defined PUMP_HAS_CPP11) 
#include <cstddef>
#endif // (defined PUMP_HAS_CPP11)
#include "pump_core/logger/pump_core_logger.h"
#include "pump_memory/pump_memory_config.h"
#include "pump_memory/pump_memory_def.h"
#include "pump_memory/smart_ptr/block.hpp"
#include "pump_memory/smart_ptr/policy.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"

namespace Pump {
namespace Memory {

const unsigned short MEM_PACK_LEN = 8;

class _Del {
public:
    void operator()(Block *p) 
    {
        if (p != 0) {
            //      pre_del(p);
            p->deallocate();
            post_del(data_);
        }
    }

public:
    virtual void pre_del(const Block *p) 
    {}

    virtual void post_del(size_t data) 
    {}

    size_t data_;
};

class VoidWPtr;

bool operator==(VoidWPtr const &a, VoidWPtr const &b);
bool operator!=(VoidWPtr const &a, VoidWPtr const &b);
bool operator==(VoidWPtr const &a, const NullPtr& null);
bool operator!=(VoidWPtr const &a, const NullPtr& null);
bool operator==(const NullPtr& null, VoidWPtr const &a);
bool operator!=(const NullPtr& null, VoidWPtr const &a);

class VoidSPtr {
private:
    typedef VoidSPtr this_type;
    typedef VoidWPtr this_weak_t;
public:
    VoidSPtr()
        : m_block(),
        m_policy(),
        m_wpParent() 
    {}

    explicit VoidSPtr(const NullPtr &)
        : m_block(),
        m_policy(),
        m_wpParent() 
    {}

    template<class A = std::allocator<char> >
    VoidSPtr(size_t n, A &a, VoidWPtr parent = VoidWPtr())
        : m_block(n, a),
        m_policy(m_block),
        m_wpParent(parent) {
    }

    VoidSPtr(const VoidWPtr &r, const NullPtr & ref, VoidWPtr parent = VoidWPtr())
        : m_block(),
        m_policy(r.m_policy, ref),
        m_wpParent(parent) {
        if (!m_policy.empty()) {
            m_block = r.m_block;
        }
    }

    //VoidSPtr(const VoidWPtr &r, const NullPtr &, VoidWPtr parent = VoidWPtr())
    //    : m_block(),
    //    m_policy(r.m_policy, PUMP_NULLPTR),
    //    m_wpParent(parent) {
    //    if (!m_policy.empty()) {
    //        m_block = r.m_block;
    //    }
    //}

    VoidSPtr(size_t n, VoidWPtr parent = VoidWPtr())
        : m_block(n),
        m_policy(m_block),
        m_wpParent(parent) {
    }

    template<class A = std::allocator<char>, class D>
    VoidSPtr(size_t n, A &a, D d, VoidWPtr parent = VoidWPtr())
        : m_block(n, a),
        m_policy(m_block, d),
        m_wpParent(parent) {
    }

    template<class D>
    VoidSPtr(size_t n, NullPtr, D d, VoidWPtr parent = VoidWPtr())
        : m_block(n),
        m_policy(m_block, d),
        m_wpParent(parent) {
    }

    explicit VoidSPtr(VoidSPtr const &r)
        : m_block(r.m_block),
        m_policy(r.m_policy),
        m_wpParent(r.m_wpParent) {
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

    VoidSPtr(VoidSPtr &&r)
        : m_block(r.m_block),
        m_policy(r.m_policy),
        m_wpParent(r.m_wpParent) {
    }

#endif //BOOST_NO_CXX11_RVALUE_REFERENCES

    virtual ~VoidSPtr() {
#ifdef _TEST_LEVEL_INFO
        PUMP_CORE_INFO << "~VoidSPtr()";
#endif //_TEST_LEVEL_INFO
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

    //  VoidSPtr &operator=(VoidSPtr &&r)  {
    //#ifdef _TEST_LEVEL_DEBUG
    //    assert(m_block.m_iCapacity >= m_block.m_iSize);
    //#endif //_TEST_LEVEL_DEBUG
    //
    //    if(m_pParent!= nullptr) {
    //      // 跟新指针的入度&出度
    //      this->m_pParent->m_policy.addOutdegree(r.get());
    //      r.m_policy.addIndegree(this->m_pParent->get());
    //    }
    //
    //    this_type(static_cast< VoidSPtr && >(r)).swap(*this);
    //
    //    return *this;
    //  }

#endif //BOOST_NO_CXX11_RVALUE_REFERENCES

    VoidSPtr &operator=(VoidSPtr const &r) {
#ifdef _TEST_LEVEL_DEBUG
        assert(m_block.capacity() >= m_block.capacity());
#endif //_TEST_LEVEL_DEBUG

        //    if(m_pParent!= nullptr) {
        //      // 跟新指针的入度&出度
        //      m_pParent->m_policy.addOutdegree(r.get());
        //      const_cast<VoidSPtr &>(r).m_policy.addIndegree(m_pParent->get());
        //    }

        this_type(r).swap(*this);

        return *this;
    }

        //  // [deprecate] 直接内存拷贝不符合智能指针标准, 因为复制内容可能包含智能指针, 直接拷贝不会增加引用计数
        //  template<class T>
        //  VoidSPtr &operator=(const T &right) {
        //    size_t iSize = sizeof(T);
        //    if (iSize > this->m_iCapacity) {
        //      // FIXME 抛出异常, 或者其他识别错误的方案
        //      throw 1;
        //    }
        //    ::memset(m_px, 0, m_iCapacity);
        //    ::memcpy(m_px, &right, iSize);
        //    return *this;
        //  }

        virtual void reset() {
        this_type().swap(*this);
    }

        void reset(VoidSPtr &other) {
        other.swap(*this);
    }

    size_t capacity() const {
        return m_block.capacity();
    }

    size_t size() const {
        return m_block.size();
    }

    void swap(VoidSPtr &r) {
        m_block.swap(r.m_block);
        m_policy.swap(r.m_policy);
        m_wpParent.swap(r.m_wpParent);
    }

        long use_count() const // nothrow
    {
        return m_policy.use_count();
    }

    bool unique() const {
        return m_policy.unique();
    }

    enum RelativeType relative(VoidSPtr const &r) const {
        return m_block.relative(r.m_block);
    }

    VoidWPtr parent() {
        return m_wpParent;
    }

    template<class T>
    T *get() const {
        return m_block.get<T>();
    }

    void *get() const {
        return (m_block.get());
    }

protected:
    friend class VoidWPtr;

    template<class Y> friend
    class WeakPtr;

    //protected: // FIXME 应该是protected
public:
    template<class T>
    T *r_get() {
        return m_block.r_get<T>();
    }

public:
    // FIXME [NORMAL] 需要一个功能更加强大的元素访问接口,应该能支持多维数组
    //  template<class T>
    //  T &operator[](size_t indx) {
    //
    //  }

protected:
    /**
     * @var m_block
     * @brief 托管内存对象
     */
    HeapGuider<> m_block;
    /**
     * @var m_policy
     * @brief 托管策略对象
     */
    SHeapPolicyGuider<> m_policy;
    /**
     * FIXME [FIXED] 研究使用VoidWPtr替换
     * @var m_pParent [试用阶段]
     * @brief 父指针对象, 父指针托管的内存成员中包含本指针托管的内存
     */
    this_weak_t m_wpParent;
};

//template<class E, class T, class Y>
//std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, VoidSPtr const & p) {
//  os << "[" << std::endl;
//  if (p == nullptr) {
//    os << "]" << std::endl;
//    return os;
//  }
//  unsigned char *szBegin = p.get<unsigned char>();
//  for (size_t i = 0, j = 0; i < p.capacity(); ++i) {
//    unsigned char ch = *szBegin;
//    if (j == 0) {
//      os << "0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned long>(szBegin);
//    }
//    os << std::hex << std::setw(8) << std::setfill('0') << static_cast<short>(ch)
//       << (j == MEM_PACK_LEN - 1 ? "\n" : "");
//    j = (j == MEM_PACK_LEN - 1 ? 0 : j + 1);
//    szBegin++;
//  }
//  return os;
//}




}
}
#endif //PUMP_VOIDSPTR2_HPP
