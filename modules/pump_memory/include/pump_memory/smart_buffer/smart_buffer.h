/**
 * @file Buffer.h
 * @brief ���� libpump ��� IO ������ģ��
 *
 * @author YangZheng 263693992@qq.com
 * @version 1.0
 * @date 2018.08.12
 */

#ifndef LIBPUMP_BUFFER_H
#define LIBPUMP_BUFFER_H

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <cstring>
#include <deque>
#include <ctime>
#include <iostream>

//#include <boost/thread/mutex.hpp>
//#include <boost/atomic.hpp>
//#include <boost/scoped_ptr.hpp>
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/thread/pump_core_atomic_op.hpp"
#include "pump_core/logger/pump_core_logger.h"
#include "kmp.h"

//namespace nsp_boost = boost;
//using namespace Pump::Core;

//template class PUMP_CORE_CLASS Pump::Core::Thread::CAtomicOpGuilder2<long long>;

namespace Pump
{
namespace Memory
{

/**
 Buffer* @class Buffer
 * @brief ����������
 * @tparam _Elem Ԫ������
 * @tparam _Alloc �ڴ������, Ĭ��Ϊ std::allocator<_Elem>
 *
 * - ���ڻ���IO����, ʹ�ö���������ȳ��ڴ��洢����, ��ʹ���ڴ������
 * ���ͷ��ڴ�, ����˽�std::string���ߵ�IO����.
 *
 * - IO����������FIFO����, ���ڲ��÷������ڴ��洢����, ����ʵ�ֶ�/д��
 * ��ȫ����, ����/д�����ɲ���, ��˶�/д������ռ��һ��������. ����������
 * ��С��Ա������ԭ����������ʵ��, ��֤��ȷ��.
 */
template<class _Elem,
class _Alloc = std::allocator<_Elem> >
class smart_buffer
{
public:
    typedef long long SizeType;
    typedef ::Pump::Core::Thread::CAtomicOpGuilder2<SizeType> AtomOpGuildType;
private:
    smart_buffer();

public:
    explicit smart_buffer(size_t iBufLen, _Alloc *alloc = NULL);

    virtual ~smart_buffer();

    /**
     * @fn long append(const char * szSrc, const size_t iLen)
     * @brief �򻺳���ĩβдһ������
     * @param szSrc [in] �����ڴ��ַ
     * @param iSLen [in] ��Ч����
     * @return 0-�ɹ���1-ʧ��
     */
    int append(const char *szSrc, size_t iSLen);

    /**
     * @fn long erase(size_t iLen)
     * @brief �ӻ�����ͷɾ��һ������
     * @param iLen [in] ȡ���ݳ���
     * @return 0-�ɹ���1-ʧ��
     */
    int erase(size_t iLen);

    SizeType find(const char *szSrc, size_t iSLen);

    SizeType size() const;

    size_t getChunkSize() const { return m_iChunkSize; }

    bool lockAppend();

    bool unlockAppend();

    bool lockErase();

    bool unlockErase();

protected:
    /**
     * @var _Alloc * const m_alloc
     * @brief �ڴ������
     * ֻ��ʹ��(��������), �������ͷ�
     */
    _Alloc *const m_alloc;

    /**
     * @defgroup Buffer::storage
     * @{
     */

    /**
     * @var std::deque<_Elem *> m_vBufs
     * @brief ��������ַ����
     *
     * ��Ҫ�󻺳�����, ��Ҫ��ÿһ�黺������С��ͬ
     */
    std::deque<_Elem *> m_vBufs;

    /**
     * @var const size_t m_iChunkSize
     * @brief һ�黺��������Ч����, ���һ��Ԫ��ʼ��δ0
     */
    const size_t m_iChunkSize;

    /**
     * @var size_t m_iBegin
     * @brief �������е���Ч��ʼλ��
     *
     * �������Ч��ʼ��ַ���ܲ���0, 0 <= m_iBegin <= m_iChunkSize
     */
    size_t m_iBegin;

    /**
     * @var boost::atomic_int64_t m_aOpBufSize
     * @brief ��������Ч���ݳ���
     */
    //nsp_boost::atomic_uint64_t m_aOpBufSize;
    AtomOpGuildType m_aOpBufSize;

    /**
     * @var boost::mutex m_mtxAppend
     * @brief append ��������
     */
    //nsp_boost::mutex m_mtxAppend;
    ::Pump::Core::Thread::CMutex m_mtxAppend;

    /**
     * @var boost::mutex m_mtxErase
     * @brief erase ��������
     */
    //nsp_boost::mutex m_mtxErase;
    ::Pump::Core::Thread::CMutex m_mtxErase;
    /** @} */

    /**
     * @var KMP m_kmp
     * @brief KMP �㷨ʵ��
     */
    kmp m_kmp;
};

template<class _Elem, class _Alloc>
smart_buffer<_Elem, _Alloc>::smart_buffer()
    : m_alloc(new std::allocator<_Elem>())
    , m_iChunkSize(0)
    , m_iBegin(0)
    , m_aOpBufSize(0)
{

}

template<class _Elem, class _Alloc>
smart_buffer<_Elem, _Alloc>::smart_buffer(size_t iBufLen, _Alloc *alloc)
    :m_alloc(m_alloc == NULL ? (new std::allocator<_Elem>()) : alloc)
    , m_iChunkSize(iBufLen)
    , m_iBegin(0)
    , m_aOpBufSize(0)
{
}

template<class _Elem, class _Alloc>
smart_buffer<_Elem, _Alloc>::~smart_buffer()
{
}

template<class _Elem, class _Alloc>
int smart_buffer<_Elem, _Alloc>::append(const char *szSrc, size_t iSLen)
{
#ifdef _TEST_LEVEL_DEBUG
    assert(m_iBegin <= m_iChunkSize);
#endif //_TEST_LEVEL_DEBUG
    const char *szWork = szSrc;
    long iRestLen = static_cast<long>(iSLen);
    double tDiff1 = 0;

    lockAppend();
    if (!m_vBufs.empty())
    {
        char *szBuf = m_vBufs.back();
        size_t iSize = strlen(szBuf);
        if (iSize < m_iChunkSize)
        {
            if ((size_t)iRestLen <= m_iChunkSize - iSize)
            {
                strncat(szBuf + iSize, szWork, static_cast<size_t>(iRestLen));
                iRestLen -= iRestLen;
            }
            else
            {
                strncat(szBuf + iSize, szWork, m_iChunkSize - iSize);
                szWork += m_iChunkSize - iSize;
                iRestLen -= (m_iChunkSize - iSize);
            }
        }
    }

    while (iRestLen > 0)
    {
#ifdef _TEST_LEVEL_DEBUG
        clock_t start, end;
        start = clock();
#endif // _TEST_LEVEL_DEBUG
        //char * szNewBuf = new char[m_iChunkSize + 1];
        char *szNewBuf = m_alloc->allocate(m_iChunkSize + 1);
        if (szNewBuf == NULL)
        {
            break;
        }
#ifdef _TEST_LEVEL_DEBUG
        end = clock();
        tDiff1 += (double)(end - start) / CLOCKS_PER_SEC;
#endif // _TEST_LEVEL_DEBUG
        memset(szNewBuf, 0, m_iChunkSize + 1);
        strncat(szNewBuf, szWork, iRestLen - m_iChunkSize >= 0 ? m_iChunkSize : iRestLen);
        szWork += (iRestLen - m_iChunkSize >= 0 ? m_iChunkSize : iRestLen);
        iRestLen -= (iRestLen - m_iChunkSize >= 0 ? m_iChunkSize : iRestLen);
        m_vBufs.push_back(szNewBuf);
    }
    m_aOpBufSize += iSLen;
    unlockAppend();
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "new op cost: " << tDiff1;
#endif // _TEST_LEVEL_DEBUG

#ifdef _TEST_LEVEL_DEBUG
    assert(m_iBegin <= m_iChunkSize);
#endif //_TEST_LEVEL_DEBUG

    return 0;
}

template<class _Elem, class _Alloc>
long long smart_buffer<_Elem, _Alloc>::find(const char *szSrc, size_t iSLen)
{
    // FIXME ������ȷӦ��������ʼλ�ò���, ��Ϊ m_vBufs ����Ч��ʼλ�ò�һ����0
    long long ret = -1;
    lockErase(); // ����ʱֻ��������,������ɾ
    ret = m_kmp.apply(m_vBufs, m_iChunkSize, szSrc, iSLen);
    unlockErase();
    return ret;
}

template<class _Elem, class _Alloc>
int smart_buffer<_Elem, _Alloc>::erase(size_t iLen)
{
#ifdef _TEST_LEVEL_DEBUG
    assert(m_iBegin <= m_iChunkSize);
#endif //_TEST_LEVEL_DEBUG
    if (iLen==0)
    {
        return 0;
    }
    long iRestDelLen = static_cast<long>(iLen);

    size_t iBufSize = (size_t)size();
    if (iLen > iBufSize)
    {
        iRestDelLen = iBufSize;
    }
    lockErase();
    while ((iRestDelLen > 0) && (!m_vBufs.empty()))
    {
        char *pWork = m_vBufs.front();
        if (iRestDelLen >= (m_iChunkSize - m_iBegin))
        {
            /* ��Ҫɾ��һ�������������ϵ���� */
            m_vBufs.pop_front();
            m_alloc->deallocate(pWork, m_iChunkSize + 1);
            m_aOpBufSize -= (AtomOpGuildType::DataType)(m_iChunkSize - m_iBegin); // ��������������Ч����
            iRestDelLen -= (m_iChunkSize - m_iBegin);
            m_iBegin = 0;
            continue;
        }
        else
        {
            /* ��Ҫɾ������һ�������������� */
            m_iBegin += iRestDelLen;
            m_aOpBufSize -= iRestDelLen;
            iRestDelLen -= iRestDelLen;
            if (m_aOpBufSize == 0)
            {
                m_vBufs.pop_front();
                m_alloc->deallocate(pWork, m_iChunkSize + 1);
                m_iBegin = 0;
            }
        }
    }
    unlockErase();
#ifdef _TEST_LEVEL_DEBUG
    assert(m_iBegin <= m_iChunkSize);
#endif //_TEST_LEVEL_DEBUG
    return 0;
}

template<class _Elem, class _Alloc>
long long smart_buffer<_Elem, _Alloc>::size() const
{
    //  size_t iSize = 0;
    //  for (typename std::deque<_Elem *>::iterator it = m_vBufs.begin();
    //       it != m_vBufs.end(); it++) {
    //    size_t iBufSize = strlen(*it) - m_iBegin;
    //    iSize += iBufSize;
    //  }
    //  return iSize;
    return m_aOpBufSize.Value();
}

template<class _Elem, class _Alloc>
bool smart_buffer<_Elem, _Alloc>::lockErase()
{
    m_mtxErase.Lock();
    return true;
};

template<class _Elem, class _Alloc>
bool smart_buffer<_Elem, _Alloc>::unlockErase()
{
    m_mtxErase.Unlock();
    return true;
}

template<class _Elem, class _Alloc>
bool smart_buffer<_Elem, _Alloc>::lockAppend()
{
    m_mtxAppend.Lock();
    return true;
}

template<class _Elem, class _Alloc>
bool smart_buffer<_Elem, _Alloc>::unlockAppend()
{
    m_mtxAppend.Unlock();
    return true;
}

/**
 * @class class IoBuffer []
 * @brief Io ����������, Buffer<> ȫ�ػ�����
 *
 * ----------------------------------------------------
 * Change History:
 * 20180902 [yangzheng] [add] long get(char * pOBuf, size_t iLen)
 */
class IoBuffer
    : public smart_buffer<char>
{
public:
    explicit IoBuffer(size_t iBufLen);

    ~IoBuffer();

    /**
     * @fn long clone(char outBuf, size_t iLen);
     * @brief �ӻ���������һ�����ݳ���
     * @param pOBuf ���������
     * @param iLen ����������
     * @return ʵ�ʿ�������
     */
    long long IoBuffer::clone(nsp_std::string &strOBuf, size_t iLen)
    {
        if (iLen <= 0)
        {
            return -1;
        }
        if (m_aOpBufSize.Value() == 0)
        {
            return 0;
        }

        long long iRestLen, iGetSize;
        iRestLen = iGetSize = static_cast<long long>(iLen);
        if (iRestLen > m_aOpBufSize.Value())
        {
            iRestLen = iGetSize = (long long)m_aOpBufSize.Value();
        }
        nsp_std::deque<char *>::iterator it = m_vBufs.begin();
        char *pszBegin = (*it) + m_iBegin;
        if (iRestLen < (m_iChunkSize - m_iBegin))
        {
            strOBuf.append(pszBegin, static_cast<size_t>(iRestLen));
            goto RET_GOOD;
        }
        strOBuf.append(pszBegin, (m_iChunkSize - m_iBegin));
        iRestLen -= (m_iChunkSize - m_iBegin);
        ++it;
        while (iRestLen > 0)
        {
            pszBegin = (*it);
            if (iRestLen < (m_iChunkSize))
            {
                strOBuf.append(pszBegin, static_cast<size_t>(iRestLen));
                goto RET_GOOD;
            }
            strOBuf.append(pszBegin, m_iChunkSize);
            iRestLen -= (m_iChunkSize);
            ++it;
        }

    RET_GOOD:
        return iGetSize;
    }

    long long IoBuffer::clone(char * pOBuf, size_t dwSize);
};

//typedef nsp_boost::shared_ptr<IoBuffer> PtrIoBuffer;

}
}

#endif //LIBPUMP_BUFFER_H
