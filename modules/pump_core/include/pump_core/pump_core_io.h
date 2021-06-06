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

class PUMP_CORE_CLASS CStreamBase
    : public CObjectBase
{
public:
    CStreamBase(const char * name, CObjectPrivateBase * priv);
    virtual ~CStreamBase();
public:
    virtual CStreamBufferBase * GetBuffer() = 0;
    virtual const CStreamBufferBase * GetCBuffer() const = 0;
    virtual void SetBuffer(const CStreamBufferBase * pbuffer) = 0;
private:
    CStreamBase();
};

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

/**
 * Not thread-safe.
 */
class PUMP_CORE_CLASS CStringBuffer
    : public CStreamBufferBase
{
public:
    CStringBuffer();
    CStringBuffer(SizeType rollSize, SizeType capacity);
    CStringBuffer(const CStringBuffer & other);
    virtual ~CStringBuffer();
public:
    CStringBuffer & operator=(const CStringBuffer & other);
public:
    virtual void* GetPtr();
    virtual const void* GetCPtr() const;
    virtual SizeType GetBufferSize() const;
    virtual int SetBufferSize(SizeType n, unsigned char ch = 0);
    virtual SizeType GetBufferRollSize() const;
    virtual SizeType GetBufferCapacity() const;
    virtual void Clear();
    virtual void Fill(const void* ptr);
    virtual void Seek(SizeType pos);
    virtual void* Anchor(SizeType pos);
public:
    static const unsigned int BufferItemSize = sizeof(char);
};

/**
 * Not thread-safe.
 */
class PUMP_CORE_CLASS CString
    : public CStreamBase
{
public:
    typedef size_t SizeType;
    typedef char* ReferenceType;
    typedef const char* ConstReferenceType;
public:
    CString();
    explicit CString(const CString& str);
    explicit CString(const CStringBuffer& str);
    virtual ~CString();
public:
    ReferenceType operator[] (SizeType pos);
    ConstReferenceType operator[] (SizeType pos) const;
    CString & operator+= (const CString& str);
    CString & operator+= (const char* s);
    CString & operator+= (char c);
    CString & operator= (const CString& str);
    CString & operator= (const CStringBuffer& str);
    CString & operator+(const CString& lhs);
    CString & operator+(const char* lhs);

    SizeType Size() const;
    int Resize(SizeType n, unsigned char ch=0);
    SizeType Capacity() const;
    void Clear();
    // CStringBuffer ShallowClone() const; // TODO 20201222 CStringBuffer need adding counter feature.
    CString& DeepClone(char* s, SizeType len, SizeType pos = 0) const;
    CStringBuffer DeepClone(SizeType pos = 0) const;
    pump_bool_t Empty() const;
    void PushBack(char c);
    void PopBack();
    CString& Formate(const char* formate, ...);
    CString& Append(const CString& str);
    CString& Append(const CString& str, SizeType subpos, SizeType sublen);
    CString& Append(const char* s);
    CString& Append(const char* s, SizeType n);
    CString& Append(SizeType n, char c);
    CString& Assign(const CString& str);
    CString& Assign(const CString& str, SizeType subpos, SizeType sublen);
    CString& Assign(const char* s);
    CString& Assign(const char* s, SizeType n);
    CString& assign(SizeType n, char c);
    CString& Insert(SizeType pos, const CString& str);
    CString& Insert(SizeType pos, const CString& str, SizeType subpos, SizeType sublen);
    CString& Insert(SizeType pos, const char* s);
    CString& Insert(SizeType pos, const char* s, SizeType n);
    CString& Insert(SizeType pos, SizeType n, char c);
    CString& Erase(SizeType pos = 0, SizeType len = CString::npos);
    CString& Replace(SizeType pos, SizeType len, const CString& str);
    CString& Replace(SizeType pos, SizeType len, const CString& str, SizeType subpos, SizeType sublen);
    CString& Replace(SizeType pos, SizeType len, const char* s);
    CString& Replace(SizeType pos, SizeType len, const char* s, SizeType n);
    CString& Replace(SizeType pos, SizeType len, SizeType n, char c);
    const char* Data() const;
    SizeType Find(const CString& str, SizeType pos = 0) const;
    SizeType Find(const char* s, SizeType pos = 0) const;
    SizeType Find(const char* s, SizeType pos, SizeType n) const;
    SizeType Find(char c, SizeType pos = 0) const;
    SizeType Rfind(const CString& str, SizeType pos = npos) const;
    SizeType Rfind(const char* s, SizeType pos = npos) const;
    SizeType Rfind(const char* s, SizeType pos, SizeType n) const;
    SizeType Rfind(char c, SizeType pos = npos) const;
    SizeType FindFirstOf(const CString& str, SizeType pos = 0) const;
    SizeType FindFirstOf(const char* s, SizeType pos = 0) const;
    SizeType FindFirstOf(const char* s, SizeType pos, SizeType n) const;
    SizeType FindFirstOf(char c, SizeType pos = 0) const;
    SizeType FindLastOf(const CString& str, SizeType pos = npos) const;
    SizeType FindLastOf(const char* s, SizeType pos = npos) const;
    SizeType FindLastOf(const char* s, SizeType pos, SizeType n) const;
    SizeType FindLastOf(char c, SizeType pos = npos) const;
    SizeType FindFirstNotOf(const CString& str, SizeType pos = 0) const;
    SizeType FindFirstNotOf(const CString* s, SizeType pos = 0) const;
    SizeType FindFirstNotOf(const char* s, SizeType pos, SizeType n) const;
    SizeType FindFirstNotOf(char c, SizeType pos = 0) const;
    SizeType FindLastNotOf(const CString& str, SizeType pos = npos) const;
    SizeType FindLastNotOf(const char* s, SizeType pos = npos) const;
    SizeType FindLastNotOf(const char* s, SizeType pos, SizeType n) const;
    SizeType FindLastNotOf(char c, SizeType pos = npos) const;
    CStringBuffer Substr(SizeType pos = 0, SizeType len = npos) const;
    int Compare(const CString& str) const;
    int Compare(SizeType pos, SizeType len, const CString& str) const;
    int Compare(SizeType pos, SizeType len, const CString& str, SizeType subpos, SizeType sublen) const;
    int Compare(const char* s) const;
    int Compare(SizeType pos, SizeType len, const char* s) const;
    int Compare(SizeType pos, SizeType len, const char* s, SizeType n) const;

private:
    virtual CStreamBufferBase * GetBuffer() { return NULL; }
    virtual const CStreamBufferBase * GetCBuffer() const { return NULL; }
    virtual void SetBuffer(const CStreamBufferBase * pbuffer) {}
public:
    static const size_t npos = -1;
};

}
}


#endif //PUMP_CORE_IO_H

