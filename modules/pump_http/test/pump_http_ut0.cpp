#include "pump_core/pump_core_app.h"
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_logger.h"
#include "pump_event/event_loop.h"
#include "pump_event/event_handler.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_http/pump_http_server.h"
#include "pump_test/pump_test.h"

using namespace ::Pump::Core;
using namespace ::Pump::Event;
using namespace Pump::Test;

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen);

PTEST_C_SCENE_DEF(PumpHttpScene000,
public:
    virtual int Init();
    virtual int Cleanup();
    pump_handle_t GetLogHandle() const;
private:
    pump_handle_t m_hLog = NULL;
    CApplication m_app;
)

int PTEST_SCENE_NAME(PumpHttpScene000)::Init()
{
    PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "PUMP_CORE_Init failed 3");
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
int PTEST_SCENE_NAME(PumpHttpScene000)::Cleanup()
{
    PTEST_ASSERT((m_hLog != PUMP_INVALID_HANDLE && m_hLog != NULL), "invalid m_hLog");
    PTEST_ASSERT((PUMP_CORE_LoggerDestroy(m_hLog) == PUMP_OK), "PUMP_CORE_LoggerDestroy failed");
    return 0;
}

pump_handle_t PTEST_SCENE_NAME(PumpHttpScene000)::GetLogHandle() const 
{ return m_hLog; }

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

PTEST_C_CASE_DEF(PumpHttpUnitTestCase000, PumpHttpScene000, )
{
    PTEST_LOG(comment, "PumpHttpUnitTestCase000 test network init & cleanup");
    PTEST_ASSERT((PUMP_CORE_CleanupNetwork() == PUMP_ERROR), "PUMP_CORE_InitNetwork failed 001");
    PTEST_ASSERT((PUMP_CORE_InitNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 001");
    PTEST_ASSERT((PUMP_CORE_CleanupNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 002");
    PTEST_ASSERT((PUMP_CORE_CleanupNetwork() == PUMP_ERROR), "PUMP_CORE_InitNetwork failed 003");
    PTEST_ASSERT((PUMP_CORE_InitNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 002");
    PTEST_ASSERT((PUMP_CORE_CleanupNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 004");
    return 0;
}

PTEST_C_CASE_DEF(PumpHttpUnitTestCase001, PumpHttpScene000, )
{
    PTEST_LOG(comment, "PumpHttpUnitTestCase001 test tcp server listening");
    PTEST_ASSERT((PUMP_CORE_InitNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 001");
    ::Pump::Event::CEventLoop * loop1 = new ::Pump::Event::CEventLoop();
    PTEST_ASSERT((loop1->Start() == PUMP_OK), "CEventLoop Start failed 001");
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    arg.emBackendType_ = ::Pump::Core::Net::PUMP_SELECT;
    ::Pump::Event::CEventListener * pFdListener = Pump::Event::HandleEvent::CHandleEventListener::getInstance(loop1->GetEventCollector(), &arg, sizeof(arg));
    loop1->InsertListener((pump_listener_handle)pFdListener);
    ::Pump::Http::CHttpServerAcceptHandler objAcceptHandler;
    objAcceptHandler.SetEventLoop(loop1);
    ::Pump::Http::CHttpServerAcceptHandler::CTCPSockAcceptArgs objArgs;
    strcpy(objArgs.szIp_, "127.0.0.1");
    objArgs.sPort = 10000;
    PTEST_ASSERT((objAcceptHandler.Open(&objArgs) == PUMP_OK), "objAcceptHandler Open failed 001");
    PUMP_CORE_Sleep(10000);
    delete loop1;
    PTEST_ASSERT((PUMP_CORE_CleanupNetwork() == PUMP_OK), "PUMP_CORE_InitNetwork failed 001");
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
    return getchar();
PTEST_MAIN_END