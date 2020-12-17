#pragma once
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_pipe.h"
#include "pump_core/pump_core_thread.h"

#define THREAD_NO 1

class CAsyncPipeServerRecvThread;

class CAsyncPipeServerThread : public ::Pump::Core::Thread::CThread
{
public:
    friend class CAsyncPipeServerRecvThread;
public:
    CAsyncPipeServerThread();
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData);
private:
    ::Pump::Core::CPipeHandle * m_pHPipe;
    HANDLE m_hCompletion;
    CAsyncPipeServerRecvThread * m_hThreadArray[THREAD_NO];
};