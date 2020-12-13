#include <iostream>
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_plugin/pump_pslot.h"
#include "pump_plugin/pump_plugin_log.h"
#include "pump_plugin/__pump_plugin_global_ctrl.h"
#include "pump_test/pump_test.h"

using namespace Pump::Test;

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen);

PTEST_C_SCENE_DEF(PumpPluginScene000, 
public:
    virtual int Init()
    {
        PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 3");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.pfnLog = Test_CB_WriteLog;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_USER);
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
        m_hLog = hLog;
        return 0;
    }
    virtual int Cleanup()
    {
        PTEST_ASSERT((m_hLog != PUMP_INVALID_HANDLE && m_hLog != NULL), "invalid m_hLog");
        PTEST_ASSERT((PUMP_CORE_LoggerDestroy(m_hLog)), "PUMP_CORE_LoggerDestroy failed");
        PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 1");
        return 0;
    }
    pump_handle_t GetLogHandle() const { return m_hLog; }
private:
    pump_handle_t m_hLog = NULL;
)

PTEST_C_CASE_DEF(PumpPluginUnitTestCase001, PumpPluginScene000, )
{
    PTEST_LOG(comment, "PumpPluginUnitTestCase001 test plugin load & unload");
    PUMP_PSLOT_CONF struConf = { 0 };
    strcpy(struConf.szInstallPath, "E:/VMware/YZ/github/Pump/modules/pump_plugin/test/test_plugin/build/");
    PTEST_ASSERT((PUMP_PSLOT_Config(&struConf) == PUMP_ERROR), "PUMP_PSLOT_Config failed 001");
    PTEST_ASSERT((PUMP_PSLOT_Cleanup() == PUMP_OK), "PUMP_PSLOT_Cleanup failed 001");
    PTEST_ASSERT((PUMP_PSLOT_Init() == PUMP_OK), "PUMP_PSLOT_Init failed 001");
    PTEST_ASSERT((PUMP_PSLOT_Init() == PUMP_OK), "PUMP_PSLOT_Init failed 001");
    PTEST_ASSERT((PUMP_PSLOT_Config(&struConf) == PUMP_OK), "PUMP_PSLOT_Config failed 002");
    PTEST_ASSERT((PUMP_PSLOT_Cleanup() == PUMP_OK), "PUMP_PSLOT_Cleanup failed 001");
    PTEST_ASSERT((PUMP_PSLOT_Config(&struConf) == PUMP_ERROR), "PUMP_PSLOT_Config failed 003");
    return 0;
}

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen)
{
    switch (emLogLevel)
    {
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_ERROR:
        PTEST_LOG(log, "[TestLog-E] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_WARNING:
        PTEST_LOG(log, "[TestLog-W] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_INFO:
        PTEST_LOG(log, "[TestLog-I] %s\n", szMessage);
        break;
    }
}

PTEST_C_CASE_DEF(PumpPluginUnitTestCase002, PumpPluginScene000, )
{
    PTEST_LOG(comment, "PumpPluginUnitTestCase002 test plugin load & unload");
    PTEST_ASSERT(PUMP_PSLOT_Init()==PUMP_OK, "PUMP_PSLOT_Init failed 001");
    PUMP_PSLOT_CONF struConf = { 0 };
    strcpy(struConf.szInstallPath, "E:/VMware/YZ/github/Pump/modules/pump_plugin/test/test_plugin/build/");
    PTEST_ASSERT(PUMP_PSLOT_Config(&struConf)==PUMP_OK, "PUMP_PSLOT_Config failed 001");
    PTEST_ASSERT(PUMP_PSLOT_InjectLocalLogger(((PTEST_SCENE_NAME(PumpPluginScene000) *)m_pScene)->GetLogHandle()) == PUMP_OK,
        "PUMP_PSLOT_InjectLocalLogger failed 001");

    pump_plugin_id id = PUMP_PSLOT_PluginLoad("test_plugin", "0.9.1");
    PTEST_ASSERT((id != -1), "load plugin failed");
    PTEST_LOG(log, "**** load plugin succeed ****");
    PUMP_CORE_Sleep(5000);
    
    PUMP_PPLUG_REQUEST struReq = { 0 };
    struReq.req_msg = new char[5];
    memset(struReq.req_msg, 0, 5);
    strcpy(struReq.req_msg, "stop");
    struReq.req_size = 5;
    PTEST_ASSERT(PUMP_PSLOT_PluginRequest(id, &struReq)==PUMP_OK, "PUMP_PSLOT_PluginRequest failed 001");
    PUMP_CORE_Sleep(5000);

    PTEST_ASSERT(PUMP_PSLOT_PluginUnload(id) == PUMP_OK, "PUMP_PSLOT_PluginUnload failed 001");

    PTEST_ASSERT(PUMP_PSLOT_Cleanup() == PUMP_OK, "PUMP_PSLOT_Cleanup failed 001");
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
    return getchar();
PTEST_MAIN_END