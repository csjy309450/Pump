#ifndef PUMP_CORE_IO_H
#define PUMP_CORE_IO_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_object.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CStreamBufferBase
    : public CObjectBase
{
public:
    typedef size_t SizeType;
public:
    CStreamBufferBase(const char * name, CObjectPrivateBase * priv);
    virtual ~CStreamBufferBase();
public:
    virtual void* GetPtr() = 0;
    virtual const void* GetCPtr() const = 0;
    virtual SizeType GetBufferSize() const = 0;
    virtual SizeType GetBufferRollSize() const = 0;
    virtual SizeType GetBufferCapacity() const = 0;
    virtual void Clear() = 0;
    virtual void Fill(const void* ptr) = 0;
    virtual void Seek(SizeType pos) = 0;
private:
    CStreamBufferBase();
};

//class PUMP_CORE_CLASS CStreamBase
//    : public CObjectBase
//{
//public:
//    CStreamBase();
//    CStreamBase(const CStreamBase & other);
//    CStreamBase & operator=(const CStreamBase & other);
//    virtual ~CStreamBase();
//public:
//    virtual CStreamBufferBase * GetBuffer() = 0;
//    virtual const CStreamBufferBase * GetCBuffer() const = 0;
//    virtual void SetBuffer(const CStreamBufferBase * pbuffer) = 0;
//};

//class CIOStream
//    : public CStreamBase
//{
//public:
//    CIOStream();
//    virtual ~CIOStream();
//}; 
//
//class CFileStream
//    : public CStreamBase
//{
//public:
//    CFileStream();
//    virtual ~CFileStream();
//};

class PUMP_CORE_CLASS CStringBuffer
    : public CStreamBufferBase
{
public:
    CStringBuffer();
    CStringBuffer(SizeType rollSize, SizeType capacity);
    virtual ~CStringBuffer();
public:
    virtual void* GetPtr();
    virtual const void* GetCPtr() const;
    virtual SizeType GetBufferSize() const;
    virtual SizeType GetBufferRollSize() const;
    virtual SizeType GetBufferCapacity() const;
    virtual void Clear();
    virtual void Fill(const void* ptr);
    virtual void Seek(SizeType pos);
};

//class PUMP_CORE_CLASS CString
//    : public CStreamBase
//{
//public:
//    CString();
//    virtual ~CString();
//};

}
}


#endif //PUMP_CORE_IO_H

