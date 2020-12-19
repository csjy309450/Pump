#ifndef PUMP_CORE_ALLOCATOR_H
#define PUMP_CORE_ALLOCATOR_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_nonop.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CAllocator
    : public CNonNewOperator
{
public:
    CAllocator();
    virtual ~CAllocator();

    static void* New(size_t size);
    static void Delete(void* ptr, size_t size);
};

}
}

#endif // PUMP_CORE_ALLOCATOR_H
