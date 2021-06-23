#ifndef PUMP_AC_NODE
#define PUMP_AC_NODE

#include "pump_macro/pump_pre.h"
#include <string>
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_ac/AC_Types.h"

namespace Pump
{
namespace Ac
{

typedef enum tagPUMP_NODE_VALUE_TYPE
{
    PUMP_NODE_VALUE_BUILDIN = 0,
    PUMP_NODE_VALUE_JSON,
    PUMP_NODE_VALUE_XML,
} PUMP_NODE_VALUE_TYPE;

class __CNodeValue;
class __CNodeRelation;

class PUMP_AC_CLASS CNode
{
public:
    friend class __CNodeValue;
    friend class __CNodeRelation;
    friend class CJsonDeserializer;
    friend class CDeserializer;
    friend class CDocument;
    friend class CDocumentPrivare;
public:
    typedef enum tagPUMP_NODE_TYPE
    {
        PUMP_NODE_WITNESS = 0,
        PUMP_NODE_NULL,
        PUMP_NODE_INT,
        PUMP_NODE_FLOAT,
        PUMP_NODE_STR,
        PUMP_NODE_BOOL,
        PUMP_NODE_ARRAY,
        PUMP_NODE_OBJECT,
    } PUMP_NODE_TYPE;
public:
    const char * getName() const;
    PUMP_NODE_TYPE getType() const;
    pump_int64_t getValueAsInt() const;
    void setValueFromInt(pump_int64_t value);
    pump_float64_t getValueAsFloat() const;
    void setValueFromFloat(pump_float64_t value);
    const char* getValueAsString() const;
    void setValueFromString(const char* value, pump_size_t iSize);

    static CNode * GetParentNode(CNode * pNode);
    static CNode * GetPreBrother(CNode * pNode);
    static CNode * GetPostBrother(CNode * pNode);
    static CNode * GetFirstSonNode(CNode * pNode);
    static CNode * GetLastSonNode(CNode * pNode);
    static CNode * CreateSonNodeInteger(pump_int64_t value);
    static CNode * CreateSonNodeFloat(pump_float64_t value);
    static CNode * CreateSonNodeString(const char* value, size_t iSize);
    static CNode * CreateSonNodeObject();
    static CNode * CreateSonNodeArray();
    static void DestroyNode(CNode * pNode); // remove all son nodes
protected:
    CNode();
    CNode(PUMP_NODE_TYPE emType, const char * key, pump_size_t iSize, __CNodeValue * pValue = NULL);
    ~CNode();
    static __CNodeValue * CreateNodeValue(PUMP_NODE_VALUE_TYPE type, void* pValue);
private:
    CNode(const CNode & other);
    CNode & operator=(CNode & other);

    CNode * __getParentNode();
    CNode * __getFirstSonNode();
    void __refreshSonNodes();
    CNode * __getLastSonNode();
    void __addSonNodeList(CNode * pNode);
protected:
    char m_key[64];
    PUMP_NODE_TYPE m_emType;
    __CNodeValue * m_pValue;
    __CNodeRelation * m_pRelation;
};

}
}


#endif // PUMP_AC_NODE
