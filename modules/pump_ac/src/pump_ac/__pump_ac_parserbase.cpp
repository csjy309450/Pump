#include "pump_ac/__pump_ac_parserbase.h"

namespace Pump
{
namespace Ac
{

CParserBase::CParserBase()
{
}


CParserBase::~CParserBase()
{
}

pump_int32_t CJsonParser::parse(const char * szBuff, size_t iSize)
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

CNode * CJsonParser::root()
{
    CNode * pNode = new CNode(CNode::PUMP_NODE_OBJECT, NULL, 0, 
        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &m_jsonRoot));
    return pNode;
}

}
}