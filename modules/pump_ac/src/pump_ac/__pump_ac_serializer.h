#ifndef PUMP_AC_SERIALIZER
#define PUMP_AC_SERIALIZER

#include <string>
#include "pump_core/pump_core_nonop.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CSerializer
    : public ::Pump::Core::CNonCopyOperator
{
public:
    explicit CSerializer(CNode** pRoot);
    ~CSerializer();
    virtual std::string dump() = 0;

private:
    CSerializer();

protected:
    CNode** m_pRoot; /// not owner, could not free.
};

class CJsonSerializer
    : public CSerializer
{
public:
    explicit CJsonSerializer(CNode** pRoot);
    virtual std::string dump();
};

}
}

#endif // PUMP_AC_SERIALIZER
