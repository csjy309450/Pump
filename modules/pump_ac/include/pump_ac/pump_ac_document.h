#ifndef PUMP_AC_DOCUMENT
#define PUMP_AC_DOCUMENT

#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_object.h"
#include "pump_ac/pump_ac_node.h"
#include "pump_ac/AC_Types.h"

namespace Pump
{
namespace Ac
{

typedef enum tagPUMP_DOC_TYPE
{
    PUMP_DOC_DEFAULT = 0,
    PUMP_DOC_JSON,
    PUMP_DOC_XML,
    PUMP_DOC_INI,
    PUMP_DOC_YAML,
} PUMP_DOC_TYPE;

class PUMP_AC_CLASS CDocument
    : public ::Pump::Core::CNonCopyOperator
    , public ::Pump::Core::CObjectBase
{
public:
    CDocument();
    CDocument(PUMP_DOC_TYPE emDocType);

    pump_int32_t parse(const char * szBuff, size_t iSize);

    CNode * root();
    CNode * get(const char* szXPath, size_t iSize);

    virtual ~CDocument();
};

}
}

#endif // !PUMP_AC_DOCUMENT
