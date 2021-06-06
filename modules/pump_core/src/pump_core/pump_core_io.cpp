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

CStreamBase::CStreamBase(const char * name, CObjectPrivateBase * priv)
    : CObjectBase(name, PUMP_FALSE, priv)
{

}

CStreamBase::~CStreamBase()
{

}

class __CStringBufferPriv
    : public CObjectPrivateBase
{
public:
    friend class CStringBuffer;
public:
    __CStringBufferPriv(size_t roll, size_t capacity = 0)
        : CObjectPrivateBase()
        , m_bufferSize(0)
        , m_bufferCapacity(capacity)
        , m_rollSize(roll)
    {
        int nRoll = capacity / m_rollSize + 1;
        for (int i = 0; i < nRoll; ++i)
        {
            unsigned char * p = (unsigned char *)CAllocator::New(m_rollSize);
            if (p)
            {
                memset(p, 0, m_rollSize);
                m_buffer.push_back(p);
            }
        }
    }

    __CStringBufferPriv(const __CStringBufferPriv & other)
        : CObjectPrivateBase()
        , m_bufferSize(other.m_bufferSize)
        , m_bufferCapacity(other.m_bufferCapacity)
        , m_rollSize(other.m_rollSize)
    {
        for (std::vector<unsigned char*>::const_iterator it = other.m_buffer.cbegin(); it != other.m_buffer.cend(); ++it)
        {
            unsigned char * p = (unsigned char *)CAllocator::New(m_rollSize);
            if (p)
            {
                memcpy(p, *it, m_rollSize);
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
                CAllocator::Delete(*it, m_rollSize);
                *it = NULL;
            }
        }
        m_buffer.clear();
    }

private:
    __CStringBufferPriv & operator=(const __CStringBufferPriv & other);

private:
    std::vector<unsigned char*> m_buffer;
    size_t m_bufferSize;
    size_t m_bufferCapacity;
    const size_t m_rollSize;
};

CStringBuffer::CStringBuffer()
    : CStreamBufferBase("CStringBuffer", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringBufferPriv(128))
{

}

CStringBuffer::CStringBuffer(SizeType rollSize, SizeType capacity)
    : CStreamBufferBase("CStringBuffer", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringBufferPriv(rollSize, capacity))
{

}

CStringBuffer::CStringBuffer(const CStringBuffer & other)
    : CStreamBufferBase("CStringBuffer", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringBufferPriv(*(__CStringBufferPriv*)other.m_pPrimitive))
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
    return ((__CStringBufferPriv*)m_pPrimitive)->m_bufferSize;
}

int CStringBuffer::SetBufferSize(SizeType n, unsigned char ch)
{
    if (!m_pPrimitive)
    {
        return -1;
    }
    __CStringBufferPriv* pStrPriv = (__CStringBufferPriv*)m_pPrimitive;
    if (pStrPriv->m_bufferSize > n)
    {
    }
    else if (pStrPriv->m_bufferSize < n)
    {
    }
    return 0;
}

CStringBuffer::SizeType CStringBuffer::GetBufferRollSize() const
{
    return ((__CStringBufferPriv*)m_pPrimitive)->m_rollSize;
}

CStringBuffer::SizeType CStringBuffer::GetBufferCapacity() const
{
    return ((__CStringBufferPriv*)m_pPrimitive)->m_bufferCapacity;
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

void* CStringBuffer::Anchor(SizeType pos)
{
    __CStringBufferPriv* pBufferPriv = (__CStringBufferPriv*)m_pPrimitive;
    if (!pBufferPriv || pos >= pBufferPriv->m_bufferSize)
    {
        return NULL;
    }
    size_t nRoll = pos / pBufferPriv->m_rollSize
        , nRollPos = pos % pBufferPriv->m_rollSize;
    if (nRoll >= pBufferPriv->m_buffer.size())
    {
        return NULL;
    }
    return (void *)(pBufferPriv->m_buffer[nRoll])[nRollPos];
}

class __CStringPriv
    : public CObjectPrivateBase
{
public:
    friend class CString;
public:
    __CStringPriv() {}
    explicit __CStringPriv(const __CStringPriv & other)
        : m_strBuffer(other.m_strBuffer)
    {}
    explicit __CStringPriv(const CStringBuffer & other)
        : m_strBuffer(other)
    {}

private:
    CStringBuffer m_strBuffer;
};

CString::CString()
    : CStreamBase("CStreamBase", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringPriv())
{

}

CString::CString(const CString& str)
    : CStreamBase("CStreamBase", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringPriv(*(__CStringPriv*)str.m_pPrimitive))
{

}

CString::CString(const CStringBuffer& str)
    : CStreamBase("CStreamBase", new(PUMP_CORE_ALLOC_MEMPOLL)__CStringPriv(str))
{

}

CString::~CString()
{
    delete m_pPrimitive;
}

CString::ReferenceType CString::operator[] (SizeType pos)
{
    __CStringPriv * pStrPriv = (__CStringPriv*)m_pPrimitive;
    if (!pStrPriv)
    {
        return NULL;
    }
    return (char*)pStrPriv->m_strBuffer.Anchor(pos);
}

CString::ConstReferenceType CString::operator[] (SizeType pos) const
{
    __CStringPriv * pStrPriv = (__CStringPriv*)m_pPrimitive;
    if (!pStrPriv)
    {
        return NULL;
    }
    return (const char*)pStrPriv->m_strBuffer.Anchor(pos);
}

CString & CString::operator+= (const CString& str)
{
    return *this;
}

CString & CString::operator+= (const char* s)
{
    return *this;
}

CString & CString::operator+= (char c)
{
    return *this;
}

CString & CString::operator= (const CString& str)
{
    return *this;
}

CString & CString::operator= (const CStringBuffer& str)
{
    return *this;
}

CString & CString::operator+(const CString& lhs)
{
    return *this;
}

CString & CString::operator+(const char* lhs)
{
    return *this;
}

CString::SizeType CString::Size() const
{
    __CStringPriv * pStrPriv = (__CStringPriv*)m_pPrimitive;
    if (!pStrPriv)
    {
        return 0;
    }
    return pStrPriv->m_strBuffer.GetBufferSize();
}

int CString::Resize(SizeType n, unsigned char ch)
{
    __CStringPriv * pStrPriv = (__CStringPriv*)m_pPrimitive;
    if (!pStrPriv)
    {
        return -1;
    }
    pStrPriv->m_strBuffer.SetBufferSize(n);
    return 0;
}

}
}