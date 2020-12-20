#ifndef PUMP_CORE_APP_H
#define PUMP_CORE_APP_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_object.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_logger.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CApplication
    : public CNonNewOperator
    , public CNonCopyOperator
{
public:
    CApplication();
    virtual ~CApplication();
    static pump_bool_t IsInit();
    static pump_int32_t SetLogger(CLogRecorderBase * plogger);
private:
    static ::Pump::Core::Thread::CMutex s_csCount;
    static pump_int32_t s_nCount;
};

}
}

#endif // PUMP_CORE_APP_H


