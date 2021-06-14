#include "pump_ac/pump_ac_node.h"
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

                }
            }
        } break;
        default:
            break;
        }
    }

    pump_float64_t getValueAsFloat() const;
    void setValueFromFloat(pump_float64_t value);
    const char* getValueAsString() const;
    void setValueFromString(const char* value, pump_size_t iSize);
    CNode * getFirstSonNode();
    CNode * getLastSonNode();
    CNode * getNextSonNode();
    CNode * getPrevSonNode();

public:
    PUMP_NODE_VALUE_TYPE m_type;
    void * m_pValue;
};

//////////////////////////////////////////////////////////////////////////
// CNode
//////////////////////////////////////////////////////////////////////////

CNode::CNode()
    : m_emType(CNode::PUMP_NODE_NULL)
    , m_pValue(NULL)
{
}

CNode::CNode(PUMP_NODE_TYPE emType
    , const char * key
    , pump_size_t iSize
    , __CNodeValue * pValue)
    : m_emType(emType)
    , m_pValue(pValue)
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
        delete m_pValue;
}

const char * CNode::getName() const
{
    return m_key;
}

CNode::PUMP_NODE_TYPE CNode::getType() const
{
    return m_emType;
}

CNode * CNode::getParentNode()
{
    if (m_pValue)
    {
        return m_pValue->getParentNode();
    }
    return NULL;
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
    return NULL;
}

void CNode::setValueFromString(const char* value, pump_size_t iSize)
{
}

CNode * CNode::getFirstSonNode()
{
    CNode * pNode = NULL;
    if (m_pValue && this->getType() == CNode::PUMP_NODE_OBJECT)
    {
        switch (m_pValue->m_type)
        {
        case PUMP_NODE_VALUE_JSON:
            Json::Value & pJson = (*(Json::Value *)(m_pValue->m_pValue)).get(0);
            if (pJson.isIntegral())
            {
                pNode = new CNode(CNode::PUMP_NODE_INT, pJson.getName().c_str(), pJson.getName().size(),
                    CNode::CreateNodeValue(PUMP_NODE_VALUE_JSON, &pJson));
            }
            else
            {
            }
            break;
        }
    }
    return pNode;
}

CNode * CNode::getLastSonNode()
{
    return NULL;
}

CNode * CNode::getNextSonNode()
{
    return NULL;
}

CNode * CNode::getPrevSonNode()
{
    return NULL;
}

__CNodeValue * CNode::CreateNodeValue(PUMP_NODE_VALUE_TYPE type, void* pValue)
{
    return new __CNodeValue(type, pValue);
}

}
}


