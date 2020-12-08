#include "async_pipe_server_thread.h"
#include "async_pipe_server_recv_thread.h"
#include "pump_core/logger/__pump_core_inner_logger.h"

using namespace ::Pump;
using namespace ::Pump::Core;
using namespace ::Pump::Core::Thread;

CAsyncPipeServerThread::CAsyncPipeServerThread()
    : m_pHPipe(NULL)
    , m_hCompletion(INVALID_HANDLE_VALUE)
{
    memset(m_hThreadArray, 0, sizeof(m_hThreadArray));
}

pump_void_t * CAsyncPipeServerThread::ThreadCallback(pump_void_t * pData)
{
    m_hCompletion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, THREAD_NO);
    if (m_hCompletion == NULL)
    {
        __PUMP_CORE_ERR("[Test] 完成端口创建失败");
        return NULL;
    }

    for (int i = 1; i <= THREAD_NO; i++)
    {
        m_hThreadArray[i] = new CAsyncPipeServerRecvThread(this);
        m_hThreadArray[i]->Start();
    }
    m_pHPipe = CPipeHandle::CreateNamedPipeServer(
        "\\\\.\\Pipe\\pump_core_test_fixture_1"
        , CPipeHandle::PUMP_PIPE_READ
        , PUMP_TRUE);
    if (!m_pHPipe)
    {
        __PUMP_CORE_ERR("[Test] CreateNamedPipe失败了");
        return NULL;
    }
    CPipeHandle::LPPUMP_RAW_PIPE pPipe = (CPipeHandle::LPPUMP_RAW_PIPE)m_pHPipe->GetHandle();
    m_hCompletion = ::CreateIoCompletionPort(pPipe->hNamedPipe_, m_hCompletion, (ULONG_PTR)m_pHPipe, 0);
    if (m_pHPipe->ConnectNamedPipeServer() != PUMP_OK)
    {
        __PUMP_CORE_ERR("[Test] ConnectNamedPipe失败了 %d", GetLastError());
    }
    while (1);
    return PUMP_NULL;
}
