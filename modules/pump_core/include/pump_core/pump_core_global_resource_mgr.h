//
// Created by yz on 19-10-13.
//

#ifndef PUMP_CORE_GLOBAL_RESOURCE_MGR_H
#define PUMP_CORE_GLOBAL_RESOURCE_MGR_H

#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CGlobalResourceMgr
  : public CNonCopyable
{
public:
    CGlobalResourceMgr() {}
    virtual ~CGlobalResourceMgr() {}
};

}
}

#endif //PUMP_CORE_GLOBAL_RESOURCE_MGR_H
