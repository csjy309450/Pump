//
// Created by yz on 19-10-13.
//

#include <cstdio>
#include <cstring>
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_pipe.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_test/pump_test.h"
#include "pump_core/logger/__pump_core_inner_logger.h"

#include "async_pipe_server_thread.h"
#include "async_pipe_server_recv_thread.h"

using namespace ::Pump;
using namespace ::Pump::Core;
using namespace ::Pump::Core::Thread;
using namespace ::Pump::Test;

PTEST_C_SCENE_DEF(UnitTestScene001, 
    virtual int Init();
    virtual int Cleanup();
)

int PTEST_SCENE_NAME(UnitTestScene001)::Init()
{
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 1");
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    strcpy(struLogCong.szFilePath, "");
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    pump_handle_t hLog = PUMP_CORE_LoggerCreate();
    PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 1");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 2");
    PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
    __PUMP_CORE_INFO("-------__PUMP_CORE_INFO good-------");
    return 0;
}
int PTEST_SCENE_NAME(UnitTestScene001)::Cleanup()
{
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 1");
    return 0;
}

class CSyncPipeServerThread :public CThread
{
public:
    CSyncPipeServerThread()
        : m_IsStop(PUMP_TRUE)
        , m_pHPipe(PUMP_NULL)
    {
    }
    virtual ~CSyncPipeServerThread()
    {
        m_IsStop = PUMP_TRUE;
        Stop();
    }
    virtual pump_int32_t Stop()
    {
        m_IsStop = PUMP_TRUE;
        CThread::Stop();
        CPipeHandle::DestroyPipe(m_pHPipe);
        return PUMP_OK;
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_pHPipe = CPipeHandle::CreateNamedPipeServer(
            "\\\\.\\Pipe\\pump_core_test_fixture_1"
            , CPipeHandle::PUMP_PIPE_READ
            , PUMP_FALSE);
        if (!m_pHPipe)
        {
            __PUMP_CORE_ERR("[Test] CreateNamedPipeServer Open Failed");
            return PUMP_NULL;
        }
        m_IsStop = PUMP_FALSE;
        __PUMP_CORE_INFO("[Test] CreateNamedPipeServer Open Succ");
        pump_char_t szBuff[1024] = { 0 };
        while (!m_IsStop)
        {
            memset(szBuff, 0, sizeof(szBuff));
            pump_uint32_t nRead = 0;
            m_pHPipe->Read(szBuff, sizeof(szBuff), &nRead);
            if (nRead>0)
            {
                __PUMP_CORE_INFO("[Test] pipe server recv: %s", szBuff);
            }
            else
            {
                PUMP_CORE_Sleep(500);
            }
        }
        return PUMP_NULL;
    }
private:
    pump_bool_t m_IsStop;
    CPipeHandle * m_pHPipe;
};

class CSyncPipeClientThread : public CThread
{
public:
    CSyncPipeClientThread()
        : m_IsStop(PUMP_TRUE)
        , m_pHPipe(PUMP_NULL)
    {
    }
    virtual ~CSyncPipeClientThread()
    {
        m_IsStop = PUMP_TRUE;
        Stop();
    }
    virtual pump_int32_t Stop()
    {
        m_IsStop = PUMP_TRUE;
        CThread::Stop();
        CPipeHandle::DestroyPipe(m_pHPipe);
        return PUMP_OK;
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_pHPipe = CPipeHandle::CreateNamedPipeClient(
            "\\\\.\\Pipe\\pump_core_test_fixture_1"
            , CPipeHandle::PUMP_PIPE_WRITE);
        if (!m_pHPipe)
        {
            __PUMP_CORE_ERR("[Test] CreateNamedPipeClient Open Failed");
            return PUMP_NULL;
        }
        m_IsStop = PUMP_FALSE;
        __PUMP_CORE_INFO("[Test] CreateNamedPipeClient Open Succ");
        pump_char_t * szBuff = "dir";
        while (!m_IsStop)
        {
            m_pHPipe->Write(szBuff, strlen(szBuff), NULL);
            __PUMP_CORE_INFO("[Test] pipe client send: %s", szBuff);
            PUMP_CORE_Sleep(500);
        }
        return PUMP_NULL;
    }
private:
    pump_bool_t m_IsStop;
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
            __PUMP_CORE_ERR("CreateNamedPipeClient Open Failed");
            return PUMP_NULL;
        }
        __PUMP_CORE_INFO("CreateNamedPipeClient Open Succ");
        pump_char_t * szBuff = "I am a Data!";
        while (1)
        {
            m_pHPipe->Write(szBuff, strlen(szBuff), NULL);
            __PUMP_CORE_INFO("pipe client send: %s", szBuff);
            PUMP_CORE_Sleep(1000);
        }
        return PUMP_NULL;
    }
private:
    CPipeHandle * m_pHPipe;
};

PTEST_C_CASE_DEF(UnitTestCase000, UnitTestScene001,)
{
    CSyncPipeServerThread thxServer;
    thxServer.Start();
    PUMP_CORE_Sleep(3000);
    CSyncPipeClientThread thxClient;
    thxClient.Start();
    PUMP_CORE_Sleep(20000);
    thxServer.Stop();
    thxClient.Stop();
    return 0;
}

PTEST_C_CASE_DEF(UnitTestCase001, UnitTestScene001, )
{
    //CAsyncPipeServerThread thxServer;
    //thxServer.Start();
    //PUMP_CORE_Sleep(3000);
    //CSyncPipeClientThread thxClient;
    //thxClient.Start();
    //PUMP_CORE_Sleep(30000);
    //thxServer.Stop();
    //thxClient.Stop();
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
    //test_pipe1();
    return getchar();
PTEST_MAIN_END