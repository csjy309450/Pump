#ifndef PUMP_AC_DESERIALIZER
#define PUMP_AC_DESERIALIZER

#include "pump_core/pump_core_nonop.h"
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/pump_ac_document.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CDeserializer
    : public ::Pump::Core::CNonCopyOperator
{
public:
    explicit CDeserializer(CNode** pRoot);
    ~CDeserializer();
    virtual pump_int32_t parse(const char * szBuff, size_t iSize) = 0;
    virtual CNode * __root() = 0;
private:
    CDeserializer();

protected:
    CNode** m_pRoot; /// not owner, could not free.
};

class CJsonDeserializer
    : public CDeserializer
{
public:
    explicit CJsonDeserializer(CNode** pRoot);
    virtual pump_int32_t parse(const char * szBuff, size_t iSize);
    virtual CNode * __root();
    std::string dump();
private:
    Json::Reader m_jsonReader;
    Json::Value m_jsonRoot;
};

}
}

#endif // PUMP_AC_DESERIALIZER

