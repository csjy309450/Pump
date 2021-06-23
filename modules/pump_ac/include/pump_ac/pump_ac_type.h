#ifndef PUMP_AC_TYPE
#define PUMP_AC_TYPE

#include <string>
#include "pump_core/pump_core_object.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CInteger
    : public ::Pump::Ac::CNode
{
public:
    CInteger(PUMP_NODE_VALUE_TYPE type, const char* szName, size_t iSize, void* pValue); ///< xml and json
    CInteger(const char* szName, size_t iSize, pump_int64_t iValue);
    ~CInteger();
};
//
//class CFloat
//    : public CSerializable
//{
//public:
//    CFloat();
//    ~CFloat();
//    virtual std::string dump();
//};
//
//class CString
//    : public CSerializable
//{
//public:
//    CString();
//    ~CString();
//    virtual std::string dump();
//};
//
//class CEnum
//    : public CSerializable
//{
//public:
//    CEnum();
//    ~CEnum();
//    virtual std::string dump();
//};
//
//class CObject
//    : public CSerializable
//{
//public:
//    CObject();
//    ~CObject();
//    virtual std::string dump();
//};
//
//class CArray
//    : public CSerializable
//{
//public:
//    CArray();
//    ~CArray();
//    virtual std::string dump();
//};

}
}
#endif // PUMP_AC_TYPE


