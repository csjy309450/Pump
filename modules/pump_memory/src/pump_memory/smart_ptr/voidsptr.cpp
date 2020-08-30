//
// Created by yz on 20-2-16.
//
#include "pump_memory/smart_ptr/voidsptr.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"
namespace Pump
{
namespace Memory
{

inline bool operator==(VoidSPtr const &p, const NullPtr &)
{
    return (p.get() == 0);
}

inline bool operator==(const NullPtr &, VoidSPtr const &p)
{
    return (p.get() == 0);
}

inline bool operator!=(VoidSPtr const &p, const NullPtr &)
{
    return (p.get() != 0);
}

inline bool operator!=(const NullPtr &, VoidSPtr const &p)
{
    return (p.get() != 0);
}

inline bool operator==(VoidSPtr const &a, VoidSPtr const &b)
{
    return (a.relative(b) == RELATIVE_EQUAL);
}

inline bool operator!=(VoidSPtr const &a, VoidSPtr const &b)
{
    return (a.relative(b) != RELATIVE_EQUAL);
}

std::ostream &operator<<(std::ostream &os, VoidSPtr const &p)
{
    os << "[" << std::endl;
    if (p == PUMP_NULLPTR)
    {
        os << "]" << std::endl;
        return os;
    }
    std::stringstream ss;
    unsigned char *szBegin = p.get<unsigned char>();
    for (size_t i = 0, j = 0; i < p.capacity(); ++i)
    {
        unsigned char ch = *szBegin;
        if (j == 0)
        {
            ss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << (unsigned long) (szBegin) << " ";
        }
        ss << std::hex << std::setw(4) << std::setfill('0') << static_cast<short>(ch)
           << (j == MEM_PACK_LEN - 1 ? "\n" : " ");
        j = (j == MEM_PACK_LEN - 1 ? 0 : j + 1);
        szBegin++;
    }
    ss << "\n]" << std::endl;
    os << ss.str();
    return os;
}

////////////////////////////////////////////////
//                  VoidWPtr
////////////////////////////////////////////////

VoidWPtr::VoidWPtr(VoidSPtr const &r)
    : m_block(r.m_block),
      m_policy(r.m_policy) {
}

VoidWPtr &VoidWPtr::operator=(VoidSPtr const &r) {
    m_block = r.m_block;
    m_policy = r.m_policy;
    return *this;
}

VoidSPtr VoidWPtr::lock_raw() const {
    return VoidSPtr(*this, PUMP_NULLPTR);
}

bool operator==(VoidWPtr const &a, VoidWPtr const &b) {
    VoidSPtr sp_a = a.lock_raw();
    VoidSPtr sp_b = b.lock_raw();
    return (sp_a == sp_b);
}

bool operator!=(VoidWPtr const &a, VoidWPtr const &b) {
    VoidSPtr sp_a = a.lock_raw();
    VoidSPtr sp_b = b.lock_raw();
    return (sp_a != sp_b);
}

bool operator==(VoidWPtr const &a, const NullPtr& null) {
    VoidSPtr sp_a = a.lock_raw();
    return (sp_a == null);
}

bool operator!=(VoidWPtr const &a, const NullPtr& null) {
    VoidSPtr sp_a = a.lock_raw();
    return (sp_a != null);
}

bool operator==(const NullPtr& null, VoidWPtr const &a) {
    VoidSPtr sp_a = a.lock_raw();
    return (sp_a == null);
}

bool operator!=(const NullPtr& null, VoidWPtr const &a) {
    VoidSPtr sp_a = a.lock_raw();
    return (sp_a != null);
}


////////////////////////////////////////////////
//                  VoidSPtr
////////////////////////////////////////////////
//
//VoidSPtr::VoidSPtr(const VoidWPtr &r, boost::detail::sp_nothrow_tag)
//  : /*m_pParent(0),*/
//  m_block(),
//  m_policy(r.m_policy, boost::detail::sp_nothrow_tag())
//{
//  if(!m_policy.empty()){
//    m_block = r.m_block;
//  }
//}

}
}
