#ifndef PUMP_AC_DESERIALIZER
#define PUMP_AC_DESERIALIZER

#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/pump_ac_document.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CDeserializer
{
public:
    CDeserializer();
    ~CDeserializer();
    virtual pump_int32_t parse(const char * szBuff, size_t iSize) = 0;
    virtual CNode * root() = 0;
    virtual std::string dump() = 0;
};

class CJsonDeserializer
    : public CDeserializer
{
public:
    virtual pump_int32_t parse(const char * szBuff, size_t iSize);
    virtual CNode * root();
    virtual std::string dump();
private:
    Json::Reader m_jsonReader;
    Json::Value m_jsonRoot;
};

}
}

#endif // PUMP_AC_DESERIALIZER

