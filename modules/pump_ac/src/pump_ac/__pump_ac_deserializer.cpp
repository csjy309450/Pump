#include "pump_ac/__pump_ac_deserializer.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CNode;

CDeserializer::CDeserializer()
    : m_pRoot(new CNode(CNode::PUMP_NODE_WITNESS, NULL, 0, NULL))
{
}


CDeserializer::~CDeserializer()
{
    if (m_pRoot)
    {
        CNode::DestroyNode(m_pRoot);
        m_pRoot = NULL;
    }
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
    CNode * pNode = CNode::GetFirstSonNode(m_pRoot);
    if (pNode)
    {
        return pNode;
    }
    pNode = new CNode(CNode::PUMP_NODE_OBJECT, NULL, 0,
        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &m_jsonRoot));
    m_pRoot->__addSonNodeList(pNode);
    return pNode;
}

}
}