#include <memory>
#include "pump_core/pump_core_allocator.h"

namespace Pump
{
namespace Core
{

CAllocator::CAllocator()
{
}

CAllocator::~CAllocator()
{
}

void* CAllocator::New(size_t size)
{
    std::allocator<char> alloc;
    return alloc.allocate(size);
}

void CAllocator::Delete(void* ptr, size_t size)
{
    std::allocator<char> alloc;
    alloc.deallocate((char*)ptr, size);
}

}
}
