#pragma once
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_pipe.h"
#include "pump_core/thread/pump_core_thread.h"

class CAsyncPipeServerThread;

class CAsyncPipeServerRecvThread :public ::Pump::Core::Thread::CThread
{
public:
    CAsyncPipeServerRecvThread(pump_void_t * pData)
        : CThread(pData) {}
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData);
private:
    ::Pump::Core::CPipeHandle * m_pHPipe;
};
