#include "pump_ac/pump_ac_document.h"
#include "pump_ac/__pump_ac_deserializer.h"

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
        , m_pParser(NULL)
    {

    }

    CDocumentPrivare(PUMP_DOC_TYPE m_emDocType)
        : m_emDocType(m_emDocType)
        , m_pParser(NULL)
    {
        switch (m_emDocType)
        {
        case  PUMP_DOC_JSON:
            m_pParser = new CJsonDeserializer();
        }
    }

    virtual ~CDocumentPrivare()
    {
        m_emDocType = PUMP_DOC_DEFAULT;
        if (!m_pParser)
            delete m_pParser;
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
        return m_pParser->root();
    }

private:
    PUMP_DOC_TYPE m_emDocType;
    CDeserializer * m_pParser;
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

void CDocument::free()
{

}

}
}