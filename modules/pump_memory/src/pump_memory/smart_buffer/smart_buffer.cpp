#include <memory>
#ifdef linux
#include <sys/select.h>
#include <sys/time.h>
#endif // linux
#include "pump_memory/smart_buffer/smart_buffer.h"

namespace Pump
{
namespace Memory
{
std::allocator<char> s_allocIobuf;

////////////////////////////////////////////////
//                   IoBuffer
////////////////////////////////////////////////

IoBuffer::IoBuffer(size_t iBufLen)
  : smart_buffer<char>(iBufLen, &s_allocIobuf)
{
  
}

IoBuffer::~IoBuffer()
{

}

long long IoBuffer::clone(char * pOBuf, size_t dwSize)
{
    if (!pOBuf || dwSize == 0)
    {
        return 0;
    }

    if (m_aOpBufSize.Value() == 0)
    {
        return 0;
    }

    long long iRestLen, iGetSize;
    iRestLen = iGetSize = static_cast<long long>(dwSize);
    if (iRestLen > m_aOpBufSize.Value())
    {
        iRestLen = iGetSize = (long long)m_aOpBufSize.Value();
    }
    if (iRestLen>dwSize)
    {
        iRestLen = dwSize;
    }
    std::deque<char *>::iterator it = m_vBufs.begin();
    char *pszBegin = (*it) + m_iBegin;
    if (iRestLen < (m_iChunkSize - m_iBegin))
    {
        strncat(pOBuf, pszBegin, static_cast<size_t>(iRestLen));
        goto RET_GOOD;
    }
    strncat(pOBuf, pszBegin, (m_iChunkSize - m_iBegin));
    iRestLen -= (m_iChunkSize - m_iBegin);
    ++it;
    while (iRestLen > 0)
    {
        pszBegin = (*it);
        if (iRestLen < (m_iChunkSize))
        {
            strncat(pOBuf, pszBegin, static_cast<size_t>(iRestLen));
            goto RET_GOOD;
        }
        strncat(pOBuf, pszBegin, m_iChunkSize);
        iRestLen -= (m_iChunkSize);
        ++it;
    }

RET_GOOD:
    return iGetSize;
}

}
}

