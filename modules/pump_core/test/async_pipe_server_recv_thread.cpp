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

        // �ⲿ���������˳��ź�
        if ((int)pipeNode == -1)
        {
            PUMP_CORE_ERR("[Test] �����߳��˳��ź�: %d", GetLastError());
            break;
        }
        if (!bOK)
        {
            if (dwErr == ERROR_INVALID_HANDLE)
            {

                PUMP_CORE_ERR("[Test] ��ɶ˿�ʧЧ: %d", GetLastError());
                break;
            }
            PUMP_CORE_ERR("[Test] �йܵ�����, err code: %d, threadid: %d node addr: %d" , dwErr , GetCurrentThreadId() , (int)pipeNode);
            if (pipeNode != pPipeServer->m_pHPipe)
            {
                PUMP_CORE_ERR("[Test] �ܵ������ڣ���������");
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
            //��Ȼ�����ݣ����ǲ����ڱ��δ��� ???
            //��Ϊ��ɶ˿ڻ����´δ����¼������Բ��������ﴦ�����ݣ�������ɶ˿ڵĹؼ���
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