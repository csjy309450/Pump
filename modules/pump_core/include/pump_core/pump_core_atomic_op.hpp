/**
 * @file pump_core_atomic_op.hpp
 * @brief Library Header.
 * @details Declaration of atomic operation interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_ATOMIC_OP_H
#define PUMP_CORE_ATOMIC_OP_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_mutex.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

template<typename L, typename T>
class /*PUMP_CORE_CLASS*/ CAtomicOp
{
public:
    typedef L LockerType;
    typedef T DataType;
public:
    CAtomicOp(LockerType &locker, DataType &data)
        : m_locker(locker), m_data(data) {}

    CAtomicOp(CAtomicOp &other)
        : m_locker(other.m_locker), m_data(other.m_data) {}

    ~CAtomicOp() {}

    DataType &Value() const
    {
        return m_data;
    }

    bool operator==(const DataType &data) const
    {
        return (m_data == data);
    }

    CAtomicOp &operator=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data = data;
        return *this;
    }

    CAtomicOp &operator+=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data += data;
        return *this;
    }

    CAtomicOp &operator-=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data -= data;
        return *this;
    }

    CAtomicOp &operator*=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data *= data;
        return *this;
    }

    CAtomicOp &operator/=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data /= data;
        return *this;
    }

    CAtomicOp &operator%=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data %= data;
        return *this;
    }

    CAtomicOp &operator++()
    {
        CMutexGuilder locker(this->m_locker);
        ++(this->m_data);
        return *this;
    }

    CAtomicOp &operator--()
    {
        CMutexGuilder locker(this->m_locker);
        --(this->m_data);
        return *this;
    }

private:
    CAtomicOp();

private:
    LockerType &m_locker;
    DataType &m_data;
};

template<typename L>
class /*PUMP_CORE_CLASS*/ CAtomicOp<L, bool>
{
public:
    typedef L LockerType;
    typedef bool DataType;
public:
    CAtomicOp(LockerType &locker, DataType &data)
        : m_locker(locker), m_data(data) {}

    CAtomicOp(CAtomicOp &other)
        : m_locker(other.m_locker), m_data(other.m_data) {}

    ~CAtomicOp() {}

    DataType &Value() const
    {
        return m_data;
    }

    bool operator==(const DataType &data) const
    {
        return (m_data == data);
    }

    CAtomicOp &operator=(const DataType &data)
    {
        CMutexGuilder locker(this->m_locker);
        this->m_data = data;
        return *this;
    }

private:
    CAtomicOp();

private:
    LockerType &m_locker;
    DataType &m_data;
};

template<typename T>
class /*PUMP_CORE_CLASS*/ CAtomicOpGuilder
{
public:
    typedef CMutex LockerType;
    typedef T DataType;
    typedef CAtomicOp<LockerType, DataType> AtomicOpType;
public:
    explicit CAtomicOpGuilder(DataType &data)
        : m_aOp(m_locker, data) {}

    DataType &Value() const
    {
        return m_aOp.Value();
    }

    bool operator==(const DataType &data) const
    {
        return (m_aOp == data);
    }

    CAtomicOpGuilder &operator=(const DataType &data)
    {
        m_aOp = data;
        return *this;
    }

    CAtomicOpGuilder &operator+=(const DataType &data)
    {
        m_aOp += data;
        return *this;
    }

    CAtomicOpGuilder &operator-=(const DataType &data)
    {
        this->m_aOp -= data;
        return *this;
    }

    CAtomicOpGuilder &operator*=(const DataType &data)
    {
        this->m_aOp *= data;
        return *this;
    }

    CAtomicOpGuilder &operator/=(const DataType &data)
    {
        this->m_aOp /= data;
        return *this;
    }

    CAtomicOpGuilder &operator%=(const DataType &data)
    {
        this->m_aOp %= data;
        return *this;
    }

    CAtomicOpGuilder &operator++()
    {
        ++(this->m_aOp);
        return *this;
    }

    CAtomicOpGuilder &operator--()
    {
        --(this->m_aOp);
        return *this;
    }

private:
    CMutex m_locker;
    AtomicOpType m_aOp;
};

template<>
class /*PUMP_CORE_CLASS*/ CAtomicOpGuilder<bool>
{
public:
    typedef CMutex LockerType;
    typedef bool DataType;
    typedef CAtomicOp<LockerType, DataType> AtomicOpType;
public:
    explicit CAtomicOpGuilder(DataType &data)
        : m_aOp(m_locker, data) {}

    DataType &Value() const
    {
        return m_aOp.Value();
    }

    bool operator==(const DataType &data) const
    {
        return (m_aOp == data);
    }

    CAtomicOpGuilder &operator=(const DataType &data)
    {
        m_aOp = data;
        return *this;
    }

private:
    CMutex m_locker;
    AtomicOpType m_aOp;
};

template<typename T>
class /*PUMP_CORE_CLASS*/ CAtomicOpGuilder2
{
public:
    typedef CMutex LockerType;
    typedef T DataType;
    typedef CAtomicOp<LockerType, DataType> AtomicOpType;
public:
    explicit CAtomicOpGuilder2(DataType data)
        : m_data(data), m_aOp(m_locker, m_data) {}

    explicit CAtomicOpGuilder2(CAtomicOpGuilder2 &other)
        : m_data(other.m_data),m_locker(),m_aOp(m_locker,m_data) {}

    DataType &Value() const
    {
        return m_aOp.Value();
    }

    bool operator==(const DataType &data) const
    {
        return (m_aOp == data);
    }

    CAtomicOpGuilder2 &operator=(const DataType &data)
    {
        m_aOp = data;
        return *this;
    }

    CAtomicOpGuilder2 &operator+=(const DataType &data)
    {
        m_aOp += data;
        return *this;
    }

    CAtomicOpGuilder2 &operator-=(const DataType &data)
    {
        this->m_aOp -= data;
        return *this;
    }

    CAtomicOpGuilder2 &operator*=(const DataType &data)
    {
        this->m_aOp *= data;
        return *this;
    }

    CAtomicOpGuilder2 &operator/=(const DataType &data)
    {
        this->m_aOp /= data;
        return *this;
    }

    CAtomicOpGuilder2 &operator%=(const DataType &data)
    {
        this->m_aOp %= data;
        return *this;
    }

    CAtomicOpGuilder2 &operator++()
    {
        ++(this->m_aOp);
        return *this;
    }

    CAtomicOpGuilder2 &operator--()
    {
        --(this->m_aOp);
        return *this;
    }

private:
    CMutex m_locker;
    DataType m_data;
    AtomicOpType m_aOp;
};

template<>
class /*PUMP_CORE_CLASS*/ CAtomicOpGuilder2<bool>
{
public:
    typedef CMutex LockerType;
    typedef bool DataType;
    typedef CAtomicOp<LockerType, DataType> AtomicOpType;
public:
    explicit CAtomicOpGuilder2(DataType data)
        : m_data(data), m_aOp(m_locker, m_data) {}

    explicit CAtomicOpGuilder2(CAtomicOpGuilder2 &other)
        : m_data(other.m_data), m_locker(), m_aOp(m_locker, m_data) {}

    DataType &Value() const
    {
        return m_aOp.Value();
    }

    bool operator==(const DataType &data) const
    {
        return (m_aOp == data);
    }

    CAtomicOpGuilder2 &operator=(const DataType &data)
    {
        m_aOp = data;
        return *this;
    }

private:
    CMutex m_locker;
    DataType m_data;
    AtomicOpType m_aOp;
};

}
}
}

#endif // PUMP_CORE_ATOMIC_OP_H