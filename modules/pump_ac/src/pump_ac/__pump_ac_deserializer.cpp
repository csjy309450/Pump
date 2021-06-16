#include "pump_ac/__pump_ac_deserializer.h"

namespace Pump
{
namespace Ac
{

CDeserializer::CDeserializer()
{
}


CDeserializer::~CDeserializer()
{
}

pump_int32_t CJsonDeserializer::parse(const char * szBuff, size_t iSize)
{
    try
    {
        bool ret = m_jsonReader.parse(szBuff, m_jsonRoot);
    }
    catch (Json::Exception & e)
    {
    }
    return 0;
}

std::string CJsonDeserializer::dump()
{
    return "";
}

CNode * CJsonDeserializer::root()
{
    CNode * pNode = new CNode(CNode::PUMP_NODE_OBJECT, NULL, 0, 
        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &m_jsonRoot));
    return pNode;
}

}
}