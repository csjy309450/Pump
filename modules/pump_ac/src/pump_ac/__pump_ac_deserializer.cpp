#include "pump_ac/__pump_ac_deserializer.h"
#include "pump_ac/pump_ac_node.h"

namespace Pump
{
namespace Ac
{

class CNode;

CDeserializer::CDeserializer(CNode** pRoot)
    : m_pRoot(pRoot)
{

}

CDeserializer::CDeserializer()
    : m_pRoot(NULL)
{
}


CDeserializer::~CDeserializer()
{
    if (m_pRoot)
    {
        m_pRoot = NULL;
    }
}

CJsonDeserializer::CJsonDeserializer(CNode** pRoot)
    : CDeserializer(pRoot)
{}

pump_int32_t CJsonDeserializer::parse(const char * szBuff, size_t iSize)
{
    try
    {
        bool ret = m_jsonReader.parse(szBuff, m_jsonRoot);
    }
    catch (Json::Exception & e)
    {
        return -1;
    }
    this->__root();
    return 0;
}

CNode * CJsonDeserializer::__root()
{
    if (!m_pRoot)
    {
        return NULL;
    }
    CNode * pNode = CNode::GetFirstSonNode(*m_pRoot);
    if (pNode)
    {
        return pNode;
    }
    pNode = new CNode(CNode::PUMP_NODE_OBJECT, NULL, 0,
        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &m_jsonRoot));
    (*m_pRoot)->__addSonNodeList(pNode);
    return pNode;
}

std::string CJsonDeserializer::dump()
{
    return m_jsonRoot.toStyledString();
}

}
}