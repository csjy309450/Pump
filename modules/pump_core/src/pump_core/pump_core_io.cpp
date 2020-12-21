#include <vector>
#include "pump_core/pump_core_object.h"
#include "pump_core/pump_core_io.h"
#include "pump_core/pump_core_allocator.h"

namespace Pump
{
namespace Core
{

CStreamBufferBase::CStreamBufferBase(const char * name, CObjectPrivateBase * priv)
    : CObjectBase(name, PUMP_FALSE, priv)
{}

CStreamBufferBase::~CStreamBufferBase()
{}

class __CStringBufferPriv
    : public CObjectPrivateBase
{
public:
    friend class CStringBuffer;
public:
    __CStringBufferPriv(size_t roll, size_t capacity = 0)
        : CObjectPrivateBase()
        , m_size(0)
        , m_capacity(capacity)
        , m_roll(roll)
    {
        int nRoll = capacity / m_roll + 1;
        for (int i = 0; i < nRoll; ++i)
        {
            unsigned char * p = (unsigned char *)CAllocator::New(m_roll);
            if (p)
            {
                memset(p, 0, m_roll);
                m_buffer.push_back(p);
            }
        }
    }

    ~__CStringBufferPriv()
    {
        for (std::vector<unsigned char*>::iterator it = m_buffer.begin(); it!=m_buffer.end(); ++it)
        {
            if (*it)
            {
                CAllocator::Delete(*it, m_roll);
                *it = NULL;
            }
        }
        m_buffer.clear();
    }

private:
    std::vector<unsigned char*> m_buffer;
    size_t m_size;
    size_t m_capacity;
    const size_t m_roll;
};

CStringBuffer::CStringBuffer()
    : CStreamBufferBase("CStringBuffer", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringBufferPriv(128))
{

}

CStringBuffer::CStringBuffer(SizeType rollSize, SizeType capacity)
    : CStreamBufferBase("CStringBuffer", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringBufferPriv(rollSize, capacity))
{

}

CStringBuffer::~CStringBuffer()
{
    delete m_pPrimitive;
}

void* CStringBuffer::GetPtr()
{
    if (((__CStringBufferPriv*)m_pPrimitive)->m_buffer.empty())
    {
        return NULL;
    }
    return ((__CStringBufferPriv*)m_pPrimitive)->m_buffer[0];
}

const void* CStringBuffer::GetCPtr() const
{
    if (((__CStringBufferPriv*)m_pPrimitive)->m_buffer.empty())
    {
        return NULL;
    }
    return ((__CStringBufferPriv*)m_pPrimitive)->m_buffer[0];
}

CStringBuffer::SizeType CStringBuffer::GetBufferSize() const
{
    return ((__CStringBufferPriv*)m_pPrimitive)->m_size;
}

CStringBuffer::SizeType CStringBuffer::GetBufferRollSize() const
{
    return ((__CStringBufferPriv*)m_pPrimitive)->m_roll;
}

CStringBuffer::SizeType CStringBuffer::GetBufferCapacity() const
{
    return ((__CStringBufferPriv*)m_pPrimitive)->m_capacity;
}

void CStringBuffer::Clear()
{
}

void CStringBuffer::Fill(const void* ptr)
{
}

void CStringBuffer::Seek(SizeType pos)
{
}


}
}