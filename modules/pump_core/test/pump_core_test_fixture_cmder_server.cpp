#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_core/pump_core_api.h"

class CThxCmdServer
    : public ::Pump::Core::Thread::CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_cmdServer.Open();
        return NULL;
    }
private:
    ::Pump::Core::Cmder::CCmderServer m_cmdServer;
};

CThxCmdServer g_thxCmdServer;

int test_logger()
{
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.szFilePath = "";
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    PUMP_CORE_InitLogger(&struLogCong);
    PUMP_CORE_INFO << "-------test begin-------";
    return  0;
}

int test_new_logger()
{
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.szFilePath = "";
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    pump_handle_t hLog = PUMP_CORE_CreateLogger(PUMP_LOG_RECORED_GLOG);
    if (hLog==PUMP_NULL)
    {
        return  -1;
    }
    if (PUMP_CORE_SetLogger(hLog, &struLogCong) == PUMP_ERROR)
    {
        return  -1;
    }
    

    PUMP_CORE_INFO << "-------test begin-------";
    return  0;
}

class CThxInnerCmdServer
    : public ::Pump::Core::Thread::CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_innerService.Open();
        m_innerService.Loop();
        return NULL;
    }
public:
    ::Pump::Core::Cmder::CCmderServer::__CInnerCmdService m_innerService;
};

int test_inner_cmd_pipe()
{
    CThxInnerCmdServer service;
    service.Start();
    Sleep(2000);
    while (1)
    {
        service.m_innerService.InputCommand("pwd");
        Sleep(1000);
    }
    return 0;
}

int main(int argc, char** argv)
{
    test_logger();
    if (g_thxCmdServer.Start() != PUMP_OK)
    {
        printf("[YZ] g_cmdServer.Open() failed!\n");
        return getchar();
    }
    printf("[YZ] 输入任意键结束\n");
    return getchar();
}