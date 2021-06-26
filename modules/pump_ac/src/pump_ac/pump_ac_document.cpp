#include "pump_ac/pump_ac_document.h"
#include "pump_ac/__pump_ac_deserializer.h"
#include "pump_ac/__pump_ac_serializer.h"

namespace Pump
{
namespace Ac
{

class CDocumentPrivare
    : public ::Pump::Core::CObjectPrivateBase
{
public:
    CDocumentPrivare()
        : m_emDocType(PUMP_DOC_DEFAULT)
        , m_pRoot(new CNode(CNode::PUMP_NODE_WITNESS, NULL, 0, NULL))
        , m_pParser(NULL)
        , m_pSerializer(NULL)
    {

    }

    CDocumentPrivare(PUMP_DOC_TYPE m_emDocType)
        : m_emDocType(m_emDocType)
        , m_pRoot(new CNode(CNode::PUMP_NODE_WITNESS, NULL, 0, NULL))
        , m_pParser(NULL)
        , m_pSerializer(NULL)
    {
        switch (m_emDocType)
        {
        case  PUMP_DOC_JSON:
            m_pParser = new CJsonDeserializer(&m_pRoot);
            m_pSerializer = new CJsonSerializer(&m_pRoot);
        }
    }

    virtual ~CDocumentPrivare()
    {
        m_emDocType = PUMP_DOC_DEFAULT;
        if (!m_pParser)
            delete m_pParser;
        if (!m_pSerializer)
        {
            delete m_pSerializer;
        }
        if (m_pRoot)
        {
            CNode::DestroyNode(m_pRoot);
        }
    }

    PUMP_DOC_TYPE getDocType()
    {
        return m_emDocType;
    }

    pump_int32_t parse(const char * szBuff, size_t iSize)
    {
        return m_pParser->parse(szBuff, iSize);
    }

    CNode * root()
    {
        if (CNode::GetFirstSonNode(m_pRoot) == NULL)
        {
            m_pParser->__root();
        }
        CNode * pNode = CNode::GetFirstSonNode(m_pRoot);
        if (!pNode)
        {
            return pNode;
        }
        CNode::GetFirstSonNode(pNode);
        return pNode;
    }

    std::string dump() 
    {
        switch (m_emDocType)
        {
        case PUMP_DOC_JSON:
            return ((CJsonDeserializer*)m_pParser)->dump();
        default:
            if (m_pSerializer)
            {
                return m_pSerializer->dump();
            }
        }
        return "";
    }

private:
    PUMP_DOC_TYPE m_emDocType;
    CNode* m_pRoot;
    CDeserializer * m_pParser;
    CSerializer * m_pSerializer;
};

CDocument::CDocument()
    : ::Pump::Core::CObjectBase("CDocument", PUMP_FALSE
        , new CDocumentPrivare())
{

}

CDocument::CDocument(PUMP_DOC_TYPE emDocType)
    : ::Pump::Core::CObjectBase("CDocument", PUMP_FALSE
        , new CDocumentPrivare(emDocType))
{

}

CDocument::~CDocument()
{
}

pump_int32_t CDocument::parse(const char * szBuff, size_t iSize)
{
    if (!szBuff || iSize == 0)
    {
        return -1;
    }
    if (this->m_pPrimitive)
    {
        return ((CDocumentPrivare*)this->m_pPrimitive)->parse(szBuff, iSize);
    }

    return -1;
}

CNode * CDocument::root()
{
    if (this->m_pPrimitive)
    {
        return ((CDocumentPrivare*)this->m_pPrimitive)->root();
    }
    return NULL;
}

CNode * CDocument::find(const char* szXPath, size_t iSize)
{
    return NULL;
}

std::string CDocument::dump() const
{
    if (this->m_pPrimitive)
    {
        return ((CDocumentPrivare*)this->m_pPrimitive)->dump();
    }
    return "";
}

}
}