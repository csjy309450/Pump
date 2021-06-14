#ifndef PUMP_AC_NODE
#define PUMP_AC_NODE

#include "pump_macro/pump_pre.h"
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

class PUMP_AC_CLASS CNode
{
public:
    friend class __CNodeValue;
    friend class CJsonParser;
public:
    typedef enum tagPUMP_NODE_TYPE
    {
        PUMP_NODE_NULL = 0,
        PUMP_NODE_INT,
        PUMP_NODE_FLOAT,
        PUMP_NODE_STR,
        PUMP_NODE_ARRAY,
        PUMP_NODE_OBJECT
    } PUMP_NODE_TYPE;
public:
    const char * getName() const;
    PUMP_NODE_TYPE getType() const;
    CNode * getParentNode();
    pump_int64_t getValueAsInt() const;
    void setValueFromInt(pump_int64_t value);
    pump_float64_t getValueAsFloat() const;
    void setValueFromFloat(pump_float64_t value);
    const char* getValueAsString() const;
    void setValueFromString(const char* value, pump_size_t iSize);
    CNode * getFirstSonNode();
    CNode * getLastSonNode();
    CNode * getNextSonNode();
    CNode * getPrevSonNode();
protected:
    CNode();
    CNode(PUMP_NODE_TYPE emType, const char * key, pump_size_t iSize, __CNodeValue * pValue = NULL);
    ~CNode();
    static __CNodeValue * CreateNodeValue(PUMP_NODE_VALUE_TYPE type, void* pValue);
private:
    CNode(const CNode & other);
    CNode & operator=(CNode & other);
protected:
    char m_key[64];
    PUMP_NODE_TYPE m_emType;
    __CNodeValue * m_pValue;
};

}
}


#endif // PUMP_AC_NODE
