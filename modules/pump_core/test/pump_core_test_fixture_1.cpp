//
// Created by yz on 19-10-13.
//

#include <cstdio>
#include <cstring>
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_pipe.h"
#include "pump_core/thread/pump_core_thread.h"

#include "async_pipe_server_thread.h"
#include "async_pipe_server_recv_thread.h"

using namespace ::Pump;
using namespace ::Pump::Core;
using namespace ::Pump::Core::Thread;

//template class PUMP_CORE_CLASS CAtomicOp<CMutex, int>;

int test_logger()
{
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.szFilePath = "";
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    //PUMP_CORE_InitLogger(&struLogCong);
    PUMP_CORE_INFO << "[Test] -------begin log-------";
    return  0;
}

class CSyncPipeServerThread :public CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_pHPipe = CPipeHandle::CreateNamedPipeServer(
            "\\\\.\\Pipe\\pump_core_test_fixture_1"
            , CPipeHandle::PUMP_PIPE_READ
            , PUMP_FALSE);
        if (!m_pHPipe)
        {
            PUMP_CORE_ERR << "[Test] CreateNamedPipeServer Open Failed";
            return PUMP_NULL;
        }
        PUMP_CORE_INFO << "[Test] CreateNamedPipeServer Open Succ";
        pump_char_t szBuff[1024] = { 0 };
        while (1)
        {
            memset(szBuff, 0, sizeof(szBuff));
            m_pHPipe->Read(szBuff, sizeof(szBuff), NULL);
            PUMP_CORE_INFO << "[Test] pipe server recv: " << szBuff;
        }
        return PUMP_NULL;
    }
private:
    CPipeHandle * m_pHPipe;
};

class CSyncPipeClientThread : public CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_pHPipe = CPipeHandle::CreateNamedPipeClient(
            "\\\\.\\Pipe\\pump_core_test_fixture_1"
            , CPipeHandle::PUMP_PIPE_WRITE);
        if (!m_pHPipe)
        {
            PUMP_CORE_ERR << "[Test] CreateNamedPipeClient Open Failed";
            return PUMP_NULL;
        }
        PUMP_CORE_INFO << "[Test] CreateNamedPipeClient Open Succ";
        pump_char_t * szBuff = "I am a Data!";
        while (1)
        {
            m_pHPipe->Write(szBuff, strlen(szBuff), NULL);
            PUMP_CORE_INFO << "[Test] pipe client send: " << szBuff;
            PUMP_CORE_Sleep(1000);
        }
        return PUMP_NULL;
    }
private:
    CPipeHandle * m_pHPipe;
};

class CAsyncPipeServerThread;

class CAsyncPipeClientThread : public CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_pHPipe = CPipeHandle::CreateNamedPipeClient(
            "\\\\.\\Pipe\\pump_core_test_fixture_1"
            , CPipeHandle::PUMP_PIPE_WRITE);
        if (!m_pHPipe)
        {
            PUMP_CORE_ERR << "CreateNamedPipeClient Open Failed";
            return PUMP_NULL;
        }
        PUMP_CORE_INFO << "CreateNamedPipeClient Open Succ";
        pump_char_t * szBuff = "I am a Data!";
        while (1)
        {
            m_pHPipe->Write(szBuff, strlen(szBuff), NULL);
            PUMP_CORE_INFO << "pipe client send: " << szBuff;
            PUMP_CORE_Sleep(1000);
        }
        return PUMP_NULL;
    }
private:
    CPipeHandle * m_pHPipe;
};

int test_pipe()
{
    CSyncPipeServerThread thxServer;
    thxServer.Start();
    PUMP_CORE_Sleep(3000);
    CSyncPipeClientThread thxClient;
    thxClient.Start();
    return getchar();
}

int test_pipe1()
{
    CAsyncPipeServerThread thxServer;
    thxServer.Start();
    PUMP_CORE_Sleep(3000);
    CSyncPipeClientThread thxClient;
    thxClient.Start();
    return getchar();
}

int main(int argc, char** argv)
{
    test_logger();
    test_pipe();
    //test_pipe1();
    return getchar();
}