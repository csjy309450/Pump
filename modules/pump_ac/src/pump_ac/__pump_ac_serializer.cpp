#include "pump_ac/__pump_ac_serializer.h"

namespace Pump
{
namespace Ac
{

CSerializer::CSerializer(CNode** pRoot)
    : m_pRoot(pRoot)
{
}


CSerializer::~CSerializer()
{
}

CJsonSerializer::CJsonSerializer(CNode** pRoot)
    : CSerializer(pRoot) {}

std::string CJsonSerializer::dump()
{
    return "";
}

}
}