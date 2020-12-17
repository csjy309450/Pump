//
// Created by yz on 19-6-8.
//

#ifndef PUMP2_CIRCULARBUFFERINTERFACE_H
#define PUMP2_CIRCULARBUFFERINTERFACE_H

#include <cstddef>
#include "pump_core/pump_core_mutex.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"

namespace Pump
{
namespace Memory
{

/**
 * @brief 
 * @tparam TBuf
 */
PUMP_INTERFACE
template<class TBuf>
class circular_buffer_interface
{
public:
  class buf_iterator
  {
  public:
    buf_iterator() : pointer_(NULL) {}
    
    ~buf_iterator() {}
    
    buf_iterator(TBuf *pointer) : pointer_(pointer) {}
    
    buf_iterator(const buf_iterator &other) : pointer_(other.pointer_) {}
    
    buf_iterator &operator=(const buf_iterator &other)
    {
      pointer_ = other.pointer_;
      return *this;
    }
    
    TBuf *get() { return pointer_; }
  
  private:
    TBuf *pointer_;
  };
  
  buf_iterator getWorkUnlocked()
  {
    return buf_iterator(m_pWork);
  }
  
  void lockWork() { m_muxWork.Lock(); }
  
  void unlock_work() { m_muxWork.Unlock(); }
  
  virtual buf_iterator get_offscreen_unlocked() = 0;
  
  void setWorkLocked(TBuf *pointer)
  {
    m_muxWork.Lock();
    m_pWork = pointer;
    m_muxWork.Unlock();
  }
  
  void setWorkUnlocked(TBuf *pointer)
  {
    m_pWork = pointer;
  }
  
  buf_iterator get_work_locked()
  {
    m_muxWork.Lock();
    return buf_iterator(m_pWork);
  }
  
  virtual void swap() = 0;

protected:
  TBuf *m_pWork; //!< 
  Pump::Core::Thread::CMutex m_muxWork; //!< pCurrBuffer
};

}
}

#endif //PUMP2_CIRCULARBUFFERINTERFACE_H
