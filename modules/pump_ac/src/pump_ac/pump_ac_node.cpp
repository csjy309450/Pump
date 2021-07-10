#include <list>
#include "pump_ac/pump_ac_node.h"
#include "pump_ac/pump_ac_type.h"
#include "pump_ac/jsoncpp/json.h"

namespace Pump
{
namespace Ac
{

class __CNodeValue
{
public:
#define __JSON_VALUE ((Json::Value *)m_pValue)
    typedef struct tag__NODE_STR_TYPE
    {
        char * szValue;
        pump_size_t iSize;
    } __NODE_STR_TYPE;

    typedef struct tag__NODE_OBJ_TYPE
    {
        CNode * pValue;
        pump_size_t iSize;
    } __NODE_OBJ_TYPE;

    typedef union tag__NODE_BUILDIN_VALUE
    {
        pump_char_t cValue; ///< null flag
        pump_int64_t iValue;
        pump_float64_t fValue;
        __NODE_STR_TYPE szValue;
        __NODE_OBJ_TYPE objValue;
    } __NODE_BUILDIN_VALUE;

    __CNodeValue(PUMP_NODE_VALUE_TYPE type)
        : m_type(type)
        , m_pValue(NULL)
    {
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            m_pValue = new __NODE_BUILDIN_VALUE();
            break;
        default:
            break;
        }
    }

    __CNodeValue(PUMP_NODE_VALUE_TYPE type, void* pValue)
        : m_type(type)
        , m_pValue(pValue)
    {
    }

    __CNodeValue(Json::Value * pJson)
        : m_type(PUMP_NODE_VALUE_JSON)
        , m_pValue(pJson)
    {
    }

    const char * getName() const
    {
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (m_pValue)
            {
                return __JSON_VALUE->getName().c_str();
            }
        } break;
        default:
            break;
        }
        return NULL;
    }

    CNode * getParentNode()
    {
        CNode * pNode = NULL;
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (m_pValue)
            {
                if (__JSON_VALUE->getParent() 
                    && (__JSON_VALUE->getParent()->isInt() || __JSON_VALUE->getParent()->isInt64()))
                {
                    pNode = new CNode(CNode::PUMP_NODE_INT, __JSON_VALUE->getName().c_str()
                        , __JSON_VALUE->getName().size(), new __CNodeValue(__JSON_VALUE->getParent()));
                }
            }
        } break;
        default:
            break;
        }
        return pNode;
    }

    pump_int64_t getValueAsInt() const
    {
        pump_int64_t value = 0;
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (__JSON_VALUE)
            {
                // [TODO] process exception
                try
                {
                    value = __JSON_VALUE->asInt64();
                }
                catch (Json::Exception & e)
                {
                    (e);
                }
            }
        } break;
        default:
            break;
        }
        return value;
    }

    void setValueFromInt(pump_int64_t value)
    {
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (__JSON_VALUE)
            {
                // [TODO] process exception
                try
                {
                    __JSON_VALUE->fromInt64(value);
                }
                catch (Json::Exception & e)
                {
                    (e);
                }
            }
        } break;
        default:
            break;
        }
    }
    pump_float64_t getValueAsFloat() const;
    void setValueFromFloat(pump_float64_t value);
    const char* getValueAsString() const
    {
        const char* value = NULL;
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (__JSON_VALUE)
            {
                // [TODO] process exception
                try
                {
                    value = __JSON_VALUE->asCString();
                }
                catch (Json::Exception & e)
                {
                    (e);
                }
            }
        } break;
        default:
            break;
        }
        return value;
    }
    void setValueFromString(const char* value, pump_size_t iSize)
    {
        switch (m_type)
        {
        case PUMP_NODE_VALUE_BUILDIN:
            break;
        case PUMP_NODE_VALUE_JSON:
        {
            if (__JSON_VALUE)
            {
                // [TODO] process exception
                try
                {
                    __JSON_VALUE->fromCString(value, (size_t)iSize);
                }
                catch (Json::Exception & e)
                {
                    (e);
                }
            }
        } break;
        default:
            break;
        }
    }
    CNode * getFirstSonNode();
    CNode * getLastSonNode();
    CNode * getNextSonNode();
    CNode * getPrevSonNode();

public:
    PUMP_NODE_VALUE_TYPE m_type;
    void * m_pValue;
};

//////////////////////////////////////////////////////////////////////////
// __CNodeRelation
//////////////////////////////////////////////////////////////////////////

class __CNodeRelation
{
public:
    __CNodeRelation()
        : m_pParentNode(NULL)
        , m_pPreBrother(NULL)
        , m_pPostBrother(NULL)
    {

    }

    class __PredRemove {
    public:
        __PredRemove(CNode * p)
            : pRemove(p)
        {

        }
        bool operator() (CNode * x) {
            return pRemove == x;
        }
    private:
        CNode * pRemove;
    };

    ~__CNodeRelation() 
    {
        for (std::list<CNode *>::iterator it = m_vecSonNode.begin(); it != m_vecSonNode.end();++it)
        {
            delete (*it);
        }
        m_vecSonNode.clear();
    }
    void setParentNode(CNode * pNode)
    {
        m_pParentNode = pNode;
    }
    CNode * getParentNode()
    {
        return m_pParentNode;
    }

    void removeFromParentSon(CNode * p)
    {
        m_pParentNode->m_pRelation->m_vecSonNode.remove_if(__PredRemove(p));
    }

    void setPreBrother(CNode * pNode)
    {
        m_pPreBrother = pNode;
    }
    CNode * getPreBrother()
    {
        return m_pPreBrother;
    }
    void setPostBrother(CNode * pNode)
    {
        m_pPostBrother = pNode;
    }
    CNode * getPostBrother()
    {
        return m_pPostBrother;
    }
    size_t getSonNodeSize() const
    {
        return m_vecSonNode.size();
    }
    void addSonNode(CNode * pNode)
    {
        m_vecSonNode.push_back(pNode);
    }
    CNode * getFirstSonNode()
    {
        if (m_vecSonNode.empty())
        {
            return NULL;
        }
        return m_vecSonNode.front();
    }
    CNode * getSonNodeByName(const char* szName, size_t iSize)
    {
        if (m_vecSonNode.empty())
        {
            return NULL;
        }
        CNode * pNode = NULL;
        for (std::list<CNode *>::iterator it = m_vecSonNode.begin(); it != m_vecSonNode.end(); ++it)
        {
            if (strcmp((*it)->getName(), szName) == 0)
            {
                pNode = (*it);
                break;
            }
        }
        return pNode;
    }
    CNode * getLastSonNode()
    {
        if (m_vecSonNode.empty())
        {
            return NULL;
        }
        return m_vecSonNode.back();
    }
private:
    CNode * m_pParentNode; /// not owner, don't free.
    CNode * m_pPreBrother; /// not owner, don't free.
    CNode * m_pPostBrother; /// not owner, don't free.
    std::list<CNode *> m_vecSonNode; /// owner, free when this free.
};

//////////////////////////////////////////////////////////////////////////
// CNode
//////////////////////////////////////////////////////////////////////////

CNode::CNode()
    : m_emType(CNode::PUMP_NODE_NULL)
    , m_pValue(NULL)
    , m_pRelation(NULL)
{
}

CNode::CNode(PUMP_NODE_TYPE emType
    , const char * key
    , pump_size_t iSize
    , __CNodeValue * pValue)
    : m_emType(emType)
    , m_pValue(pValue)
    , m_pRelation(new __CNodeRelation())
{
    memset(m_key, 0, sizeof(m_key));
    if (key && iSize > 0)
    {
        strncpy(m_key, key, sizeof(m_key));
    }
}

CNode::~CNode()
{
    if (m_pValue)
    {
        delete m_pValue;
    }
    if (m_pRelation)
    {
        delete m_pRelation;
    }
}

const char * CNode::getName() const
{
    return m_key;
}

CNode::PUMP_NODE_TYPE CNode::getType() const
{
    return m_emType;
}

pump_int64_t CNode::getValueAsInt() const
{
    if (m_pValue)
    {
        return m_pValue->getValueAsInt();
    }
    return 0;
}

void CNode::setValueFromInt(pump_int64_t value)
{
}

pump_float64_t CNode::getValueAsFloat() const
{
    return 0;
}

void CNode::setValueFromFloat(pump_float64_t value)
{
}

const char* CNode::getValueAsString() const
{
    if (m_pValue)
    {
        return m_pValue->getValueAsString();
    }
    return NULL;
}

void CNode::setValueFromString(const char* value, pump_size_t iSize)
{
    if (m_pValue)
    {
        m_pValue->setValueFromString(value, iSize);
    }
}

size_t CNode::getSonNodeCount()
{
    const CNode * pWork = this->__getFirstSonNode();
    if (pWork)
    {
        if (m_pRelation)
        {
            return this->m_pRelation->getSonNodeSize();
        }
    }
    return 0;
}

pump_bool_t CNode::isObject() const
{
    return this->getType() == CNode::PUMP_NODE_OBJECT;
}

pump_bool_t CNode::isArray() const
{
    return this->getType() == CNode::PUMP_NODE_ARRAY;
}

pump_bool_t CNode::isInteger() const
{
    return this->getType() == CNode::PUMP_NODE_INT;
}

pump_bool_t CNode::isFloat() const
{
    return this->getType() == CNode::PUMP_NODE_FLOAT;
}

pump_bool_t CNode::isNull() const
{
    return this->getType() == CNode::PUMP_NODE_NULL;
}

pump_bool_t CNode::isString() const
{
    return this->getType() == CNode::PUMP_NODE_STR;
}

pump_bool_t CNode::isBoolean() const
{
    return this->getType() == CNode::PUMP_NODE_BOOL;
}

CNode * CNode::__getParentNode()
{
    if (m_pRelation)
    {
        return this->m_pRelation->getParentNode();
    }
    return NULL;
}

void CNode::__refreshSonNodes()
{
    CNode * pNode = NULL;
    CNode * pPreNode = NULL;
    if (m_pValue && (this->isObject() || this->isArray()))
    {
        switch (m_pValue->m_type)
        {
        case PUMP_NODE_VALUE_JSON:
        {
            for (size_t i = 0; i < (*(Json::Value *)(m_pValue->m_pValue)).size(); ++i)
            {
                Json::Value & pJson = (*(Json::Value *)(m_pValue->m_pValue)).get(i);
                if (pJson.isIntegral())
                {
                    //pNode = new CNode(CNode::PUMP_NODE_INT, pJson.getName().c_str(), pJson.getName().size(),
                    //    CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                    pNode = new CInteger(PUMP_NODE_VALUE_JSON, pJson.getName().c_str(), pJson.getName().size(), &pJson);
                }
                else if (pJson.isDouble())
                {
                    pNode = new CNode(CNode::PUMP_NODE_FLOAT, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                else if (pJson.isArray())
                {
                    pNode = new CNode(CNode::PUMP_NODE_ARRAY, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                else if (pJson.isBool())
                {
                    pNode = new CNode(CNode::PUMP_NODE_BOOL, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                else if (pJson.isObject())
                {
                    pNode = new CNode(CNode::PUMP_NODE_OBJECT, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                else if (pJson.isNull())
                {
                    pNode = new CNode(CNode::PUMP_NODE_NULL, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                else if (pJson.isString())
                {
                    pNode = new CNode(CNode::PUMP_NODE_STR, pJson.getName().c_str(), pJson.getName().size(),
                        CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
                }
                pNode->m_pRelation->setParentNode(this);
                this->m_pRelation->addSonNode(pNode);
                if (pPreNode)
                {
                    pNode->m_pRelation->setPreBrother(pPreNode);
                    pPreNode->m_pRelation->setPostBrother(pNode);
                }
                pPreNode = pNode;
            }
        } break;
        }
    }
}

CNode * CNode::__getFirstSonNode()
{
    if (!this->m_pRelation->getFirstSonNode())
    {
        this->__refreshSonNodes();
    }
    return this->m_pRelation->getFirstSonNode();
}

CNode * CNode::__getLastSonNode()
{
    if (!this->m_pRelation->getLastSonNode())
    {
        this->__refreshSonNodes();
    }
    return this->m_pRelation->getLastSonNode();
}

void CNode::__addSonNodeList(CNode * pNode)
{
    pNode->m_pRelation->setParentNode(this);
    this->m_pRelation->addSonNode(pNode);
}

__CNodeValue * CNode::CreateNodeValue(PUMP_NODE_VALUE_TYPE type, void* pValue)
{
    switch (type)
    {
    case PUMP_NODE_VALUE_BUILDIN:
    {
        return new __CNodeValue(type);
    } break;
    case PUMP_NODE_VALUE_JSON:
    {
        return new __CNodeValue(type, pValue);
    } break;
    default:
        return NULL;
    }
}

void CNode::DestroyNode(CNode * pNode)
{
    if (pNode)
    {
        if (pNode->m_pRelation && pNode->m_pRelation->getParentNode())
        {
            pNode->m_pRelation->removeFromParentSon(pNode);
        }
        delete pNode;
    }
}

CNode * CNode::GetFirstSonNode(CNode * pNode)
{
    if (pNode)
    {
        return pNode->__getFirstSonNode();
    }
    return NULL;
}

CNode * CNode::GetSonNodeByIndex(CNode * pNode, size_t iIndx)
{
    CNode * pWork = NULL;
    if (!pNode || iIndx > pNode->getSonNodeCount())
    {
        return pWork;
    }
    size_t tiIndx = 0;
    pWork = pNode->__getFirstSonNode();
    while (pWork)
    {
        if (tiIndx == iIndx)
        {
            break;
        }
        pWork = CNode::GetPostBrother(pWork);
        tiIndx++;
    }
    return pWork;
}

CNode * CNode::GetSonNodeByName(CNode * pNode, const char* szName, size_t iSize)
{
    if (pNode)
    {
        CNode * pWork = pNode->__getFirstSonNode();
        if (pWork)
        {
            return pNode->m_pRelation->getSonNodeByName(szName, iSize);
        }
    }
    return NULL;
}

CNode * CNode::GetLastSonNode(CNode * pNode)
{
    if (pNode)
    {
        CNode * pWork = pNode->__getFirstSonNode();
        if (pWork)
        {
            return pNode->__getLastSonNode();
        }
    }
    return NULL;
}

CNode * CNode::GetPreBrother(CNode * pNode)
{
    if (pNode && pNode->m_pRelation)
    {
        return pNode->m_pRelation->getPreBrother();
    }
    return NULL;
}

CNode * CNode::GetPostBrother(CNode * pNode)
{
    if (pNode && pNode->m_pRelation)
    {
        return pNode->m_pRelation->getPostBrother();
    }
    return NULL;
}

CNode * CNode::GetParentNode(CNode * pNode)
{
    if (pNode)
    {
        return pNode->__getParentNode();
    }
    return NULL;
}

CNode * CNode::CreateSonNodeInteger(CNode * pNode, const char* szName, size_t iSize, pump_int64_t iValue)
{
    if (!pNode || !szName || iSize==0)
    {
        return NULL;
    }
    CNode * pNewNode = NULL;
    switch (pNode->m_pValue->m_type)
    {
    case PUMP_NODE_VALUE_JSON:
    {
        Json::Value * pjNode = (Json::Value *)pNode->m_pValue->m_pValue;
        if (!pjNode || !pjNode->isObject())
        {
            break;
        }
        Json::Value jNode(iValue);
        jNode.setName(std::string(szName));
        pjNode->resolveReference(szName) = (jNode);
        Json::Value * pNewJNode = pjNode->findByName(szName, iSize);
        if (!pNewJNode)
        {
            break;
        }
        pNewNode = new CInteger(PUMP_NODE_VALUE_JSON, szName, iSize, pNewJNode);
        if (!pNewNode)
        {
            break;
        }
        pNode->__addNewSonNode(pNewNode);
    } break;
    }
    return pNewNode;
}

void CNode::__addNewSonNode(CNode * pNode)
{
    pNode->m_pRelation->setParentNode(this);
    this->m_pRelation->addSonNode(pNode);
    CNode * pPreNode = this->m_pRelation->getLastSonNode();
    if (pPreNode)
    {
        pNode->m_pRelation->setPreBrother(pPreNode);
        pPreNode->m_pRelation->setPostBrother(pNode);
    }
}

}
}


