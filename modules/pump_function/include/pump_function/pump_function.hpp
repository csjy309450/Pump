#ifndef _PUMP_FUNCTION_H_
#define _PUMP_FUNCTION_H_
#include <cassert>
#include <iostream>
#include <vector>

// pump_function version info
#ifndef PUMP_FUNCTION_VERSION_MAJOR
#   define PUMP_FUNCTION_VERSION_MAJOR 0
#endif // !PUMP_FUNCTION_VERSION_MAJOR
#ifndef PUMP_FUNCTION_VERSION_MINOR
#   define PUMP_FUNCTION_VERSION_MINOR 0
#endif // !PUMP_FUNCTION_VERSION_MINOR
#ifndef PUMP_FUNCTION_VERSION_PATCH
#   define PUMP_FUNCTION_VERSION_PATCH 9
#endif // !PUMP_FUNCTION_VERSION_PATCH
#ifndef PUMP_FUNCTION_VERSION_BUILD
#   define PUMP_FUNCTION_VERSION_BUILD 0
#endif // !PUMP_FUNCTION_VERSION_BUILD
#define __PUMP_FUNCTION_VERSION_STR(str_) #str_
#define __PUMP_FUNCTION_VERJOIN(major_,minor_,patch_,build_) __PUMP_FUNCTION_VERSION_STR(major_.minor_.patch_.build_)
#define PUMP_FUNCTION_VERSION __PUMP_FUNCTION_VERJOIN(PUMP_FUNCTION_VERSION_MAJOR,PUMP_FUNCTION_VERSION_MINOR,PUMP_FUNCTION_VERSION_PATCH,PUMP_FUNCTION_VERSION_BUILD)

#define PUMP_GFN_TYPE(ret_, argv_) ret_(*)argv_
#define PUMP_MFN_TYPE(class_,ret_, argv_) ret_(class_::*)argv_
#define PUMP_GFN(ret_, argv_) Pump::Function::CFunction<PUMP_GFN_TYPE(ret_, argv_)>
#define PUMP_MFN(class_,ret_, argv_) Pump::Function::CFunction<PUMP_MFN_TYPE(class_, ret_, argv_)>
#define PUMP_RET(retType_,pfn_) (((Pump::Function::CFunction0<retType_>*)(pfn_))->GetReturn())
#define PUMP_RET_NONVOID(retType_,pfn_,default_) \
((pfn_)->IsRet() ? PUMP_RET(retType_,pfn_) : default_)

namespace Pump
{
namespace Function
{
#ifdef __linux__
#define __TypeName typename
#elif (defined _WIN32)
#define __TypeName 
#endif // __linux__


struct  __TrueType
{};

struct  __FalseType
{};

template<typename T>
struct __BoolType
{
    typedef __TrueType Value;
};

template<>
struct __BoolType<void>
{
    typedef __FalseType Value;
};

class CFunctionBase
{
public:
    CFunctionBase(const __TrueType & bRet, unsigned char  nArgc);
    CFunctionBase(const __FalseType & bRet, unsigned char  nArgc);
    virtual ~CFunctionBase();
    bool IsRet() const;
    const unsigned char  GetArgc() const;
protected:
    CFunctionBase();
protected:
    const bool m_bRet;
    const unsigned char m_nArgc;
};
typedef CFunctionBase* CFunctionPtr;

class CClosureBase
    : virtual public CFunctionBase
{
public:
    virtual void operator()() = 0;
    virtual void* GetReturn() = 0;
};
typedef CClosureBase* CClosurePtr;

/*
* @brief R(*)(void)原型的函数
*/
template<typename PF, typename R>
class CFunction0
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
public:
    CFunction0()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 0), m_pfn(NULL){}
    CFunction0(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 0), m_pfn(pfn){}
    virtual ~CFunction0() { }
protected:
    PFnType  m_pfn;
};

/*
* @brief R(*)(void)原型的函数
*/
template<typename PF, typename R>
class CClosure0
    : public CFunction0<PF, R>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
public:
    CClosure0()
        : CFunction0<PF, R>(NULL)
        , m_ret(NULL) {}
    CClosure0(PFnType pfn)
        : CFunction0<PF, R>(pfn)
        , m_ret(NULL) {}
    virtual ~CClosure0() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
};

/*
* @brief R(*)(void)原型的函数
*/
template<typename PF>
class CFunction0<PF, void>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
public:
    CFunction0()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 0), m_pfn(NULL){}
    CFunction0(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 0), m_pfn(pfn){}
    virtual ~CFunction0() { }
protected:
    PFnType m_pfn;
};

/*
* @brief R(*)(void)原型的函数
*/
template<typename PF>
class CClosure0<PF, void>
    : public CFunction0<PF, void>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
public:
    CClosure0()
        : CFunction0<PF, void>(NULL){}
    CClosure0(PFnType pfn)
        : CFunction0<PF, void>(pfn){}
    virtual ~CClosure0() { }
};

/*
* @brief R(*)(T1)原型的函数
*/
template<typename PF, typename R, typename T1>
class CFunction1
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;
public:
    CFunction1()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 1)
        , m_pfn(NULL){}
    CFunction1(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 1)
        , m_pfn(pfn){}
    CFunction1(const CFunction1 & other)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 1)
        , m_pfn(other.m_pfn){}
    virtual ~CFunction1() {}
    CFunction1 & operator=(const CFunction1 & other)
    {
        m_pfn = (other.m_pfn);
        return  *this;
    }
protected:
    PFnType m_pfn;
};

template<typename PF, typename R, typename T1>
class CClosure1
    : public CFunction1<PF, R, T1>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
public:
    CClosure1()
        : CFunction1<PF, R, T1>(NULL), m_ret(NULL), m_t1() {}
    CClosure1(PFnType pfn, Arg1Type t1)
        : CFunction1<PF, R, T1>(pfn), m_ret(NULL), m_t1(t1) {}
    virtual ~CClosure1() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }

protected:
    RetType *m_ret;
    Arg1Type m_t1;
};

/*
* @brief R(*)(T1)原型的函数
*/
template<typename PF, typename T1>
class CFunction1<PF, void, T1>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
public:
    CFunction1()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 1), m_pfn(NULL){}
    CFunction1(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 1), m_pfn(pfn){}
    virtual ~CFunction1() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1>
class CClosure1<PF, void, T1>
    : public CFunction1<PF, void, T1>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
public:
    CClosure1()
        : CFunction1<PF, void, T1>(NULL), m_ret(NULL), m_t1() {}
    CClosure1(PFnType pfn, Arg1Type t1)
        : CFunction1<PF, void, T1>(pfn), m_ret(pfn), m_t1(t1) {}
    virtual ~CClosure1() { }
    virtual void* GetReturn() { return m_ret; }

protected:
    RetType *m_ret;
    Arg1Type m_t1;
};

/*
* @brief R(T1, T2)原型的函数
*/
template<typename PF, typename R, typename T1, typename T2>
class CFunction2
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
public:
    CFunction2()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 2), m_pfn(NULL) {}
    CFunction2(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 2), m_pfn(pfn) {}
    virtual ~CFunction2() { }
protected:
    PFnType m_pfn;
};

template<typename PF, typename R, typename T1, typename T2>
class CClosure2
    : public CFunction2<PF, R, T1, T2>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
public:
    CClosure2()
        : CFunction2<PF, R, T1, T2>(NULL), m_ret(NULL), m_t1(), m_t2() {}
    CClosure2(PFnType pfn, Arg1Type t1, Arg2Type t2)
        : CFunction2<PF, R, T1, T2>(pfn), m_ret(NULL), m_t1(t1), m_t2(t2) {}
    virtual ~CClosure2() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType * m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
};

/*
* @brief R(T1, T2)原型的函数
*/
template<typename PF, typename T1, typename T2>
class CFunction2<PF, void, T1, T2>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
public:
    CFunction2()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 2)
        , m_pfn(NULL) {}
    CFunction2(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 2), m_pfn(pfn) {}
    virtual ~CFunction2() { }
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1, typename T2>
class CClosure2<PF, void, T1, T2>
    : public CFunction2<PF, void, T1, T2>, public CClosureBase
{
public:
    typedef PF PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
public:
    CClosure2()
        : CFunction2<PF, void, T1, T2>(NULL), m_ret(NULL)
        , m_t1()
        , m_t2() {}
    CClosure2(PFnType pfn, Arg1Type t1, Arg2Type t2)
        : CFunction2<PF, void, T1, T2>(pfn), m_ret(NULL), m_t1(t1), m_t2(t2) {}
    virtual ~CClosure2() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType * m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
};

/*
* @brief R(T1, T2, T3)原型的函数
*/
template<typename PF, typename R,
    typename T1, typename T2, typename T3>
class CFunction3
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
public:
    CFunction3()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 3)
        , m_pfn(NULL) {}
    CFunction3(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 3), m_pfn(pfn) {}
    virtual ~CFunction3() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename R,
    typename T1, typename T2, typename T3>
class CClosure3
    : public CFunction3<PF, R, T1, T2, T3>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
public:
    CClosure3()
        : CFunction3<PF, R, T1, T2, T3>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(){}
    CClosure3(PFnType pfn, Arg1Type  & t1, Arg2Type t2, Arg3Type t3)
        : CFunction3<PF, R, T1, T2, T3>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3){}
    virtual ~CClosure3() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
};

/*
* @brief R(T1, T2, T3)原型的函数
*/
template<typename PF, typename T1, typename T2, typename T3>
class CFunction3<PF, void, T1, T2, T3>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
public:
    CFunction3()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 3), m_pfn(NULL){}
    CFunction3(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 3), m_pfn(pfn){}
    virtual ~CFunction3() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1, typename T2, typename T3>
class CClosure3<PF, void, T1, T2, T3>
    : public CFunction3<PF, void, T1, T2, T3>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
public:
    CClosure3()
        : CFunction3<PF, void, T1, T2, T3>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(){}
    CClosure3(PFnType pfn, Arg1Type  & t1, Arg2Type t2, Arg3Type t3)
        : CFunction3<PF, void, T1, T2, T3>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3){}
    virtual ~CClosure3() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
};

/*
* @brief R(T1, T2, T3, T4)原型的函数
*/
template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4>
class CFunction4
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
public:
    CFunction4()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 4), m_pfn(NULL){}
    CFunction4(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 4), m_pfn(pfn){}
    virtual ~CFunction4() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4>
class CClosure4
    : public CFunction4<PF, R, T1, T2, T3, T4>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
public:
    CClosure4()
        : CFunction4<PF, R, T1, T2, T3, T4>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(){}
    CClosure4(PFnType pfn, Arg1Type t1, Arg2Type t2, Arg3Type t3,
        Arg4Type t4)
        : CFunction4<PF, R, T1, T2, T3, T4>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4){}
    virtual ~CClosure4() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
};

/*
* @brief R(T1, T2, T3, T4)原型的函数
*/
template<typename PF, typename T1, typename T2, typename T3,
    typename T4>
class CFunction4<PF, void, T1, T2, T3, T4>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
public:
    CFunction4()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 4), m_pfn(NULL){}
    CFunction4(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 4), m_pfn(pfn){}
    virtual ~CFunction4() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1, typename T2, typename T3,
    typename T4>
class CClosure4<PF, void, T1, T2, T3, T4>
    : public CFunction4<PF, void, T1, T2, T3, T4>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
public:
    CClosure4()
        : CFunction4<void, T1, T2, T3, T4>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(){}
    CClosure4(PFnType pfn, Arg1Type t1, Arg2Type t2, Arg3Type t3,
        Arg4Type t4)
        : CFunction4<void, T1, T2, T3, T4>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4){}
    virtual ~CClosure4() {}
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
};

/*
* @brief R(T1, T2, T3, T4, T5)原型的函数
*/
template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4, typename T5>
class CFunction5
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
public:
    CFunction5()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 5), m_pfn(NULL) {}
    CFunction5(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 5), m_pfn(pfn) {}
    virtual ~CFunction5() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4, typename T5>
class CClosure5
    : public CFunction5<PF, R, T1, T2, T3, T4, T5>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
public:
    CClosure5()
        : CFunction5<PF, R, T1, T2, T3, T4, T5>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(), m_t5(){}
    CClosure5(PFnType pfn, Arg1Type t1, Arg2Type  & t2, Arg3Type t3,
        Arg4Type t4, Arg5Type t5)
        : CFunction5<PF, R, T1, T2, T3, T4, T5>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5){}
    virtual ~CClosure5() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
    Arg5Type m_t5;
};

/*
* @brief R(T1, T2, T3, T4, T5)原型的函数
*/
template<typename PF, typename T1, typename T2, typename T3,
    typename T4, typename T5>
class CFunction5<PF, void, T1, T2, T3, T4, T5>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
public:
    CFunction5()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 5) 
        , m_pfn(NULL){}
    CFunction5(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 5)
        , m_pfn(pfn) {}
    virtual ~CFunction5() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1, typename T2, typename T3,
    typename T4, typename T5>
class CClosure5<PF, void, T1, T2, T3, T4, T5>
    : public CFunction5<PF, void, T1, T2, T3, T4, T5>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
public:
    CClosure5()
        : CFunction5<PF, void, T1, T2, T3, T4, T5>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(), m_t5(){}
    CClosure5(PFnType pfn, Arg1Type t1, Arg2Type  & t2, Arg3Type t3,
        Arg4Type t4, Arg5Type t5)
        : CFunction5<PF, void, T1, T2, T3, T4, T5>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5){}
    virtual ~CClosure5() {}
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
    Arg5Type m_t5;
};

/*
* @brief R(T1, T2, T3, T4, T5, T6)原型的函数
*/
template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
class CFunction6
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
    typedef T6 Arg6Type;
public:
    CFunction6()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 6) 
        m_pfn(NULL) {}
    CFunction6(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 6) 
        , m_pfn(pfn) {}
    virtual ~CFunction6() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename R,
    typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
class CClosure6
    : public CFunction6<PF, void, T1, T2, T3, T4, T5, T6>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
    typedef T6 Arg6Type;
public:
    CClosure6()
        : CFunction6<PF, void, T1, T2, T3, T4, T5, T6>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(), m_t5(), m_t6(){}
    CClosure6(PFnType pfn, Arg1Type t1, Arg2Type t2, Arg3Type t3,
        Arg4Type t4, Arg5Type t5, Arg6Type t6)
        : CFunction6<PF, void, T1, T2, T3, T4, T5, T6>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6){}
    virtual ~CClosure6() { delete m_ret; }
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
    Arg5Type m_t5;
    Arg5Type m_t6;
};

/*
* @brief R(T1, T2, T3, T4, T5, T6)原型的函数
*/
template<typename PF, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
class CFunction6<PF, void, T1, T2, T3, T4, T5, T6>
    : virtual public CFunctionBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
    typedef T6 Arg6Type;
public:
    CFunction6()
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 6), m_pfn(NULL) {}
    CFunction6(PFnType pfn)
        : CFunctionBase(__TypeName __BoolType<RetType>::Value(), 6), m_pfn(pfn) {}
    virtual ~CFunction6() {}
protected:
    PFnType m_pfn;
};

template<typename PF, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
class CClosure6<PF, void, T1, T2, T3, T4, T5, T6>
    : public CFunction6<PF, void, T1, T2, T3, T4, T5, T6>, public CClosureBase
{
public:
    typedef PF PFnType ;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;
    typedef T4 Arg4Type;
    typedef T5 Arg5Type;
    typedef T6 Arg6Type;
public:
    CClosure6()
        : CFunction6<PF, void, T1, T2, T3, T4, T5, T6>(NULL), m_ret(NULL)
        , m_t1(), m_t2(), m_t3(), m_t4(), m_t5(), m_t6(){}
    CClosure6(PFnType pfn, Arg1Type t1, Arg2Type t2, Arg3Type t3,
        Arg4Type t4, Arg5Type t5, Arg6Type t6)
        : CFunction6<PF, void, T1, T2, T3, T4, T5, T6>(pfn), m_ret(NULL)
        , m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6){}
    virtual ~CClosure6() {}
    virtual void* GetReturn() { return m_ret; }
protected:
    RetType *m_ret;
    Arg1Type m_t1;
    Arg2Type m_t2;
    Arg3Type m_t3;
    Arg4Type m_t4;
    Arg5Type m_t5;
    Arg5Type m_t6;
};

template<typename T>
class CFunction
{};

template<typename T>
class CClosure
{};

/*
* @brief R(*)(void)函数对象
*/
template<typename R>
class CFunction<R(*)(void)>
    : public CFunction0<R(*)(void), R>
{
public:
    typedef CFunction0<R(*)(void), R> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
public:
    CFunction() : SupperType::RetType>(){}
    CFunction(PFnType pfn)
        : SupperType(pfn) {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    RetType operator()()
    {
        return SupperType::m_pfn();
    }
};

template<typename R>
class CClosure<R(*)(void)>
    : public CClosure0<R(*)(void), R>
{
public:
    typedef CClosure0<R(*)(void), R> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
public:
    CClosure() : SupperType::RetType>() {}
    CClosure(PFnType pfn) : SupperType(pfn) {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        if (!SupperType::m_ret)
        {
            SupperType::m_ret = new (std::nothrow)RetType();
        }
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret = SupperType::m_pfn();
    }
};

/*
* @brief void(*)(void)函数偏特化对象
*/
template<>
class CFunction<void(*)(void)>
    : public CFunction0<void(*)(void), void>
{
public:
    typedef CFunction0<void(*)(void), void> SupperType;
    typedef SupperType::PFnType PFnType;
    typedef void RetType;
public:
    CFunction() : SupperType() {}
    CFunction(PFnType pfn)
        : SupperType(pfn) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_pfn();
    }
};

template<>
class CClosure<void(*)(void)>
    : public CClosure0<void(*)(void), void>
{
public:
    typedef CClosure0<void(*)(void), void> SupperType;
    typedef SupperType::PFnType PFnType;
    typedef void RetType;
public:
    CClosure() : SupperType() {}
    CClosure(PFnType pfn)
        : SupperType(pfn) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_pfn();
    }
};

/*
* @brief R(T::*)(void)类成员函数对象
*/
template<typename R, typename T>
class CFunction<R(T::*)(void)>
    : public CFunction0<R(T::*)(void), R>
{
public:
    typedef CFunction0<R(T::*)(void), R> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
public:
    CFunction() : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual R operator()()
    {
        return (m_pThis->*SupperType::m_pfn)();
    }
public:
    ObjType * m_pThis;
};

template<typename R, typename T>
class CClosure<R(T::*)(void)>
    : public CClosure0<R(T::*)(void), R>
{
public:
    typedef CClosure0<R(T::*)(void), R> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
public:
    CClosure() : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (CClosure0<void(*)(void), void>::m_pfn == NULL); }
    void Clear() { CClosure0<void(*)(void), void>::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        CClosure0<void(*)(void), void>::m_pfn = pfn;
        m_pThis = p;
    }
    virtual void operator()()
    {
        if (!SupperType::m_ret)
        {
            SupperType::m_ret = new (std::nothrow)RetType();
        }
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret = (m_pThis->*SupperType::m_pfn)();
    }
public:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(void)类成员函数偏特化对象
* @note T是类实例的类型，异步跨线程调用时，禁止将栈上对象的指针传入
*/
template<typename T>
class CFunction<void(T::*)(void)>
    : public CFunction0<void(T::*)(void), void>
{
public:
    typedef CFunction0<void(T::*)(void), void> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
public:
    CFunction() : SupperType(), m_pThis(NULL){}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual RetType operator()()
    {
        (m_pThis->*SupperType::m_pfn)();
    }
public:
    ObjType * m_pThis;
};

template<typename T>
class CClosure<void(T::*)(void)>
    : public CClosure0<void(T::*)(void), void>
{
public:
    typedef CClosure0<void(T::*)(void), void> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
public:
    CClosure() : SupperType(), m_pThis(NULL){}
    CClosure(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual RetType operator()()
    {
        (m_pThis->*SupperType::m_pfn)();
    }
public:
    ObjType * m_pThis;
};

/*
* @brief R(*)(T1)函数对象
*/
template<typename R, typename T1>
class CFunction<R(*)(T1)>
    : public CFunction1<R(*)(T1), R, T1>
{
public:
    typedef CFunction1<R(*)(T1), R, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;

public:
    CFunction() : SupperType(){}
    CFunction(PFnType pfn)
        : SupperType(pfn){}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType  pfn)
    {
        SupperType::m_pfn = pfn;
    }
    RetType operator()(Arg1Type t1)
    {
        return SupperType::m_pfn(t1);
    }
};

template<typename R, typename T1>
class CClosure<R(*)(T1)>
    : public CClosure1<R(*)(T1), R, T1>
{
public:
    typedef CClosure1<R(*)(T1), R, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;

public:
    CClosure() : SupperType(){}
    CClosure(PFnType pfn, Arg1Type t1)
        : SupperType(pfn, t1){}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, Arg1Type t1)
    {
        SupperType::m_pfn = pfn;
        SupperType::m_t1 = t1;
    }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_ret = new (std::nothrow)RetType();
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret
            = SupperType::m_pfn(SupperType::m_t1);
    }
};

/*
* @brief void(*)(T1)函数偏特化版本
*/
template<typename T1>
class CFunction<void(*)(T1)>
    : public CFunction1<void(*)(T1), void, T1>
{
public:
    typedef CFunction1<void(*)(T1), void, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
public:
    CFunction() : SupperType(){}
    CFunction(PFnType  pfn)
        : SupperType(pfn) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType  pfn)
    {
        SupperType::m_pfn = pfn;
    }
    RetType operator()(Arg1Type t1)
    {
        SupperType::m_pfn(t1);
    }
};

template<typename T1>
class CClosure<void(*)(T1)>
    : public CClosure1<void(*)(T1), void, T1>
{
public:
    typedef CClosure1<void(*)(T1), void, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
public:
    CClosure() : SupperType(){}
    CClosure(PFnType  pfn, Arg1Type t1)
        : SupperType(pfn, t1){}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, Arg1Type t1)
    {
        SupperType::m_pfn = pfn;
        SupperType::m_t1 = t1;
    }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_pfn(SupperType::m_t1);
    }
};

/*
* @brief R(T::*)(T1)类成员函数
*/
template<typename R,
    typename T, typename T1>
class CFunction<R(T::*)(T1)>
    : public CFunction1<R(T::*)(T1), R, T1>
{
public:
    typedef CFunction1<R(T::*)(T1), R, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType  pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    CFunction(const CFunction & other)
        : SupperType(other), m_pThis(other.m_pThis) {}
    virtual ~CFunction() {}
    CFunction & operator=(const CFunction & other)
    {
        this->m_pfn = (other.m_pfn);
        this->m_pThis = (other.m_pThis);
        return  *this;
    }
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType  pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1)
    {
        return (m_pThis->*SupperType::m_pfn)(t1);
    }
public:
    ObjType * m_pThis;
};

template<typename R,
    typename T, typename T1>
class CClosure<R(T::*)(T1)>
    : public CClosure1<R(T::*)(T1), R, T1>
{
public:
    typedef CClosure1<R(T::*)(T1), R, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p, Arg1Type t1)
        : SupperType(pfn, t1), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
    }
    virtual void operator()()
    {
        SupperType::m_ret = new (std::nothrow)RetType();
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret =
            (m_pThis->*SupperType::m_pfn)(SupperType::m_t1);
    }
public:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(T1)类成员函数偏特化版本
*/
template<typename T, typename T1>
class CFunction<void(T::*)(T1)>
    : public CFunction1<void(T::*)(T1), void, T1>
{
public:
    typedef CFunction1<void(T::*)(T1), void, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1)
    {
        (m_pThis->*SupperType::m_pfn)(t1);
    }
public:
    ObjType * m_pThis;
};

template<typename T, typename T1>
class CClosure<void(T::*)(T1)>
    : public CClosure1<void(T::*)(T1), void, T1>
{
public:
    typedef CClosure1<void(T::*)(T1), void, T1> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType  pfn, ObjType * p, Arg1Type t1)
        : SupperType(pfn, t1), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType  pfn, ObjType * p, Arg1Type t1)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
    }
    virtual void operator()()
    {
        (m_pThis->*SupperType::m_pfn)(SupperType::m_t1);
    }

public:
    ObjType * m_pThis;
};

/*
* @brief R(*)(T1, T2)函数对象
*/
template<typename R, typename T1, typename T2>
class CFunction<R(*)(T1, T2)> 
    : public CFunction2<R(*)(T1, T2), R, T1, T2>
{
public:
    typedef CFunction2<R(*)(T1, T2), R, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CFunction()
        : SupperType() {}
    CFunction(PFnType pfn)
        : SupperType(pfn) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2)
    {
        return SupperType::m_pfn(t1, t2);
    }
};

template<typename R, typename T1, typename T2>
class CClosure<R(*)(T1, T2)>
    : public CClosure2<R(*)(T1, T2), R, T1, T2>
{
public:
    typedef CClosure2<R(*)(T1, T2), R, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CClosure() : SupperType() {}
    CClosure(PFnType pfn, Arg1Type t1, Arg2Type t2)
        : SupperType(pfn, t1, t2) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn = pfn;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
    }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_ret = new (std::nothrow)RetType();
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret =
            SupperType::m_pfn(
            SupperType::m_t1,
            SupperType::m_t2);
    }
};

/*
* @brief void(*)(T1, T2)函数对象特化版本
*/
template<typename T1, typename T2>
class CFunction<void(*)(T1, T2)> 
    : public CFunction2<void(*)(T1, T2), void, T1, T2>
{
public:
    typedef CFunction2<void(*)(T1, T2), void, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
public:
    CFunction() : SupperType() {}
    CFunction(PFnType  pfn)
        : SupperType(pfn) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn(t1, t2);
    }
};

/*
* @brief void(*)(T1, T2)函数对象特化版本
*/
template<typename T1, typename T2>
class CClosure<void(*)(T1, T2)>
    : public CClosure2<void(*)(T1, T2), void, T1, T2>
{
public:
    typedef CClosure2<void(*)(T1, T2), void, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CClosure()
        : SupperType() {}
    CClosure(PFnType pfn, Arg1Type t1, Arg2Type t2)
        : SupperType(pfn, t1, t2) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn = pfn;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
    }
    void Bind(PFnType pfn)
    {
        SupperType::m_pfn = pfn;
    }
    virtual void operator()()
    {
        SupperType::m_pfn(
            SupperType::m_t1, 
            SupperType::m_t2);
    }
};

/*
* @brief R(T::*)(T1, T2)函数对象
*/
template<typename R, typename T, 
    typename T1, typename T2>
class CFunction<R(T::*)(T1, T2)>
    : public CFunction2<R(T::*)(T1, T2), R, T1, T2>
{
public:
    typedef CFunction2<R(T::*)(T1, T2), R, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2)
    {
        return (m_pThis->*SupperType::m_pfn)(t1, t2);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief R(T::*)(T1, T2)函数对象
*/
template<typename R, typename T,
    typename T1, typename T2>
class CClosure<R(T::*)(T1, T2)>
    : public CClosure2<R(T::*)(T1, T2), R, T1, T2>
{
public:
    typedef CClosure2<R(T::*)(T1, T2), R, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2)
        : SupperType(pfn, t1, t2), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
    }
    virtual void operator()()
    {
        SupperType::m_ret = new (std::nothrow)RetType();
        if (!SupperType::m_ret)
        {
            assert(1 == 0);
        }
        *SupperType::m_ret = (m_pThis->*SupperType::m_pfn)(
            SupperType::m_t1, 
            SupperType::m_t2);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(T1, T2)函数对象特化版本
*/
template<typename T,
    typename T1, typename T2>
class CFunction<void(T::*)(T1, T2)>
    : public CFunction2<void(T::*)(T1, T2), void, T1, T2>
{
public:
    typedef CFunction2<void(T::*)(T1, T2), void, T1, T2> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() {m_pfn=NULL;}
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
    }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2)
    {
        (m_pThis->*SupperType::m_pfn)(t1, t2);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(T1, T2)函数对象特化版本
*/
template<typename T,
    typename T1, typename T2>
class CClosure<void(T::*)(T1, T2)>
    : public CClosure2<void(T::*)(T1, T2), void, T1, T2>
{
public:
    typedef CClosure2<void(T::*)(T1, T2), void, T1, T2> SupperType;
    typedef typename SupperType::PFnType  PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2)
        : SupperType(pfn, t1, t2), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return ( SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
    }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual void operator()()
    {
        (m_pThis->*SupperType::m_pfn)(SupperType::m_t1, SupperType::m_t2);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief R(T::*)(T1, T2, T3)函数对象特化版本
*/
template<typename R, typename T,
    typename T1, typename T2, typename T3>
class CFunction<R(T::*)(T1, T2, T3)>
    : public CFunction3<R(T::*)(T1, T2, T3), R, T1, T2, T3>
{
public:
    typedef CFunction3<R(T::*)(T1, T2, T3), R, T1, T2, T3> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(pfn), m_pThis(p) {}
    CFunction & operator=(CFunction & other)
    {
        m_pfn = other.m_pfn;
        m_pThis = other.m_pThis;
        return *this;
    }
    virtual ~CFunction() {}
    bool Empty() const { return ( SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2, Arg3Type t3)
    {
        return (m_pThis->*SupperType::m_pfn)(t1, t2, t3);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief R(T::*)(T1, T2, T3)函数对象特化版本
*/
template<typename R, typename T,
    typename T1, typename T2, typename T3>
class CClosure<R(T::*)(T1, T2, T3)>
    : public CClosure3<R(T::*)(T1, T2, T3), R, T1, T2, T3>
{
public:
    typedef CClosure3<R(T::*)(T1, T2, T3), R, T1, T2, T3> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef R RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2, Arg3Type t3)
        : SupperType(pfn, t1, t2, t3), m_pThis(p) {}
    CClosure(CClosure & other)
        : SupperType(other.m_pfn, other.SupperType::m_t1, other.SupperType::m_t2, other.SupperType::m_t3)
        , m_pThis(other.m_pThis)
    {}
    CClosure & operator=(CClosure & other)
    {
        m_pfn = other.m_pfn;
        m_pThis = other.m_pThis;
        SupperType::m_t1 = other.SupperType::m_t1;
        SupperType::m_t2 = other.SupperType::m_t2;
        SupperType::m_t3 = other.SupperType::m_t3;
        return *this;
    }
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2, Arg3Type t3)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
        SupperType::m_t3 = t3;
    }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual void operator()()
    {
        (m_pThis->*SupperType::m_pfn)(SupperType::m_t1, SupperType::m_t2, SupperType::m_t3);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(T1, T2, T3)函数对象特化版本
*/
template<typename T,
    typename T1, typename T2, typename T3>
class CFunction<void(T::*)(T1, T2, T3)>
    : public CFunction3<void(T::*)(T1, T2, T3), void, T1, T2, T3>
{
public:
    typedef CFunction3<void(T::*)(T1, T2, T3), void, T1, T2, T3> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;

public:
    CFunction()
        : SupperType(), m_pThis(NULL) {}
    CFunction(PFnType pfn, ObjType * p)
        : SupperType(), m_pThis(p) {}
    virtual ~CFunction() {}
    bool Empty() const { return ( SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    RetType operator()(Arg1Type t1, Arg2Type t2, Arg3Type t3)
    {
        (m_pThis->*SupperType::m_pfn)(t1, t2, t3);
    }
private:
    ObjType * m_pThis;
};

/*
* @brief void(T::*)(T1, T2, T3)函数对象特化版本
*/
template<typename T,
    typename T1, typename T2, typename T3>
class CClosure<void(T::*)(T1, T2, T3)>
    : public CClosure3<void(T::*)(T1, T2, T3), void, T1, T2, T3>
{
public:
    typedef CClosure3<void(T::*)(T1, T2, T3), void, T1, T2, T3> SupperType;
    typedef typename SupperType::PFnType PFnType;
    typedef void RetType;
    typedef T ObjType;
    typedef T1 Arg1Type;
    typedef T2 Arg2Type;
    typedef T3 Arg3Type;

public:
    CClosure()
        : SupperType(), m_pThis(NULL) {}
    CClosure(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2, Arg3Type t3)
        : SupperType(pfn, t1, t2, t3), m_pThis(p) {}
    virtual ~CClosure() {}
    bool Empty() const { return (SupperType::m_pfn == NULL); }
    void Clear() { SupperType::m_pfn = NULL; }
    void Bind(PFnType pfn, ObjType * p, Arg1Type t1, Arg2Type t2, Arg3Type t3)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
        SupperType::m_t1 = t1;
        SupperType::m_t2 = t2;
        SupperType::m_t3 = t3;
    }
    void Bind(PFnType pfn, ObjType * p)
    {
        SupperType::m_pfn = pfn;
        m_pThis = p;
    }
    virtual void operator()()
    {
        (m_pThis->*SupperType::m_pfn)(SupperType::m_t1, SupperType::m_t2, SupperType::m_t3);
    }
private:
    ObjType * m_pThis;
};

}
}

#endif //_PUMP_FUNCTION_H_