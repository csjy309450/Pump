#include <queue>
#include "pump_core/pump_core_string.h"
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
    if (!m_pRoot || !(*m_pRoot))
    {
        return "";
    }
    std::string strBuff;
    CNode * pRoot = CNode::GetFirstSonNode((*m_pRoot));
    
    strBuff = "{" + __dumpObject(pRoot) + "}";
    return strBuff;
}

std::string CJsonSerializer::__dumpInt(CNode * pNode)
{
    std::string strBuff;
    if (!pNode || pNode->getType() != CNode::PUMP_NODE_INT)
    {
        return strBuff;
    }
    pump_int64_t iValue = pNode->getValueAsInt();
    strBuff += PUMP_CORE_Other2Str<pump_int64_t>(iValue);
    return strBuff;
}

std::string CJsonSerializer::__dumpObject(CNode * pNode)
{
    std::string strBuff;
    if (!pNode || pNode->getType() != CNode::PUMP_NODE_OBJECT)
    {
        return strBuff;
    }
    std::queue<CNode*> nodequeue;
    CNode * pSon = CNode::GetFirstSonNode(pNode);
    for (; !pSon; pSon = CNode::GetPostBrother(pSon))
    {
        nodequeue.push(pSon);
    }
    while (!nodequeue.empty())
    {
        CNode * pWork = nodequeue.front();
        nodequeue.pop();
        switch (pWork->getType())
        {
        case CNode::PUMP_NODE_OBJECT:
        {
            CNode * pSon = CNode::GetFirstSonNode(pWork);
            for (; !pSon; pSon = CNode::GetPostBrother(pSon))
            {
                nodequeue.push(pSon);
            }
            strBuff += "\"" + std::string(pWork->getName()) + "\": {" + this->__dumpObject(pWork) + "}";
        } break;
        case CNode::PUMP_NODE_INT:
        {
            strBuff += "\"" + std::string(pWork->getName()) + "\": {" + this->__dumpInt(pWork) + "}";
        } break;
        }
        strBuff += (nodequeue.empty() ? "" : ",");
    }
    return strBuff;
}

}
}