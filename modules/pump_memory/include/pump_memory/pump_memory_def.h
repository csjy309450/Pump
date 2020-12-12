#ifndef PUMP_MEMORY_DEF_H
#define PUMP_MEMORY_DEF_H
#include "pump_macro/pump_os_config.h"
#ifdef PUMP_OS_WIN32
#pragma warning(disable: 4521)
#endif // PUMP_OS_WIN32

namespace Pump {
namespace Memory {

#if (defined PUMP_HAS_CPP11) && (defined PUMP_MEMORY_USING_CXX11) // c++标准大于11
typedef NullPtr std::nullptr_t;
#define PUMP_NULLPTR nullptr
#else
struct _NullPtr
{
    _NullPtr() {}
    _NullPtr(const _NullPtr &) {}
    _NullPtr(int) {}
    _NullPtr(_NullPtr &) {}
    ~_NullPtr() {}
    _NullPtr & operator=(const _NullPtr &) { return *this; }
    _NullPtr & operator=(int) { return *this; }
    bool operator==(const _NullPtr &) { return true; }
#if (defined PUMP_HAS_CPP11) 
    bool operator==(const std::nullptr_t) { return true; }
#endif // (defined PUMP_HAS_CPP11)
    bool operator==(int val) { return ((val == 0) ? true : false); }
};
typedef struct _NullPtr NullPtr;
#define PUMP_NULLPTR NullPtr(0)
#endif // (defined PUMP_HAS_CPP11) || (defined PUMP_HAS_CPP14)  || (defined PUMP_HAS_CPP17)

template< class T > struct SP_Element
{
    typedef T ElmType;
};

}
}

#endif // PUMP_MEMORY_DEF_H

