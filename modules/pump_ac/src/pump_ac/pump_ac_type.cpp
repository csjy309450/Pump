#include "pump_core/pump_core_string.h"
#include "pump_ac/pump_ac_type.h"

namespace Pump
{
namespace Ac
{

CInteger::CInteger(PUMP_NODE_VALUE_TYPE type, const char* szName, size_t iSize, void* pValue)
    : CNode(CNode::PUMP_NODE_INT, szName, iSize,
    CreateNodeValue(type, pValue))
{
}

CInteger::CInteger(const char* szName, size_t iSize, pump_int64_t iValue)
    : CNode(CNode::PUMP_NODE_INT, szName, iSize,
    CreateNodeValue(PUMP_NODE_VALUE_BUILDIN, NULL))
{
    this->setValueFromInt(iValue);
}

CInteger::~CInteger()
{
}

}
}