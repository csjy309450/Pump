#include "async_pipe_server_recv_thread.h"
#include "async_pipe_server_thread.h"

using namespace ::Pump;
using namespace ::Pump::Core;
using namespace ::Pump::Core::Thread;

pump_void_t * CAsyncPipeServerRecvThread::ThreadCallback(pump_void_t * pData)
{
    CAsyncPipeServerThread * pPipeServer = (CAsyncPipeServerThread *)pData;
    DWORD dwTrans = 0, dwErr = 0;
    CPipeHandle* pipeNode = NULL;
    OVERLAPPED *pOverlapped = NULL;
    unsigned char szBuff[1024] = { 0 };
    while (TRUE)
    {
        BOOL bOK = ::GetQueuedCompletionStatus(pPipeServer->m_hCompletion, &dwTrans, (PULONG_PTR)&pipeNode, &pOverlapped, INFINITE);
        dwErr = GetLastError();
        if (pOverlapped == NULL)
        {
            PUMP_CORE_ERR("[Test] *******pOverlapped nulll: %d" , GetLastError());
            continue;
        }

        // 外部主动发起退出信号
        if ((int)pipeNode == -1)
        {
            PUMP_CORE_ERR("[Test] 工作线程退出信号: %d", GetLastError());
            break;
        }
        if (!bOK)
        {
            if (dwErr == ERROR_INVALID_HANDLE)
            {

                PUMP_CORE_ERR("[Test] 完成端口失效: %d", GetLastError());
                break;
            }
            PUMP_CORE_ERR("[Test] 有管道出错, err code: %d, threadid: %d node addr: %d" , dwErr , GetCurrentThreadId() , (int)pipeNode);
            if (pipeNode != pPipeServer->m_pHPipe)
            {
                PUMP_CORE_ERR("[Test] 管道不存在！！！！！");
            }
            continue;
        }
        auto len = pipeNode->GetOverLapped()->InternalHigh;
        auto ilen = pOverlapped->InternalHigh;

        memset(szBuff, 0, sizeof(szBuff));
        pump_uint32_t dwRead = 0;
        BOOL fSuccess = pipeNode->Read(szBuff, sizeof(szBuff) - 1, &dwRead);
        if (fSuccess &&  dwRead != 0)
        {
            //虽然有数据，但是不能在本次处理 ???
            //因为完成端口会再下次触发事件，所以不再在这里处理数据，这是完成端口的关键点
            PUMP_CORE_INFO("[Test] bSuccess-----iopending:len: %d, Thread id : %d" , dwRead , GetCurrentThreadId());
            continue;
        }
        // The read operation is still pending. 
        dwErr = GetLastError();
        if (!fSuccess && (dwErr == ERROR_IO_PENDING))
        {
            PUMP_CORE_INFO("[Test] -----iopending:len: %d,  Thread id: %d" , dwTrans ,GetCurrentThreadId());
            continue;
        }
        else
        {
            //TODO:
            PUMP_CORE_INFO("[Test] other error, error code: %d, Thread id: %d" , GetLastError() , GetCurrentThreadId());
        }
    }
    return PUMP_NULL;
}