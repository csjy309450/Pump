//
// Created by yz on 19-10-13.
//

#include <cstdio>
#include <cstring>
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_atomic_op.hpp"
#include "pump_core/pump_core_sock.h"
#include "pump_core/pump_core_cmdparse.hpp"
#include "pump_core/pump_core_environment.h"
#include "pump_core/__pump_core_inner_logger.h"
#include "pump_core/pump_core_dllso.h"
#include "pump_core/pump_core_thread_pool.h"
#include "pump_core/pump_core_app.h"
#include "pump_core/pump_core_io.h"
#include "pump_test/pump_test.h"

using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Core::Net;
using namespace Pump::Core::Thread;
using namespace Pump::Core::Cmder;
using namespace Pump::Test;

PTEST_C_SCENE_DEF(PumpCoreUnitTestScene001, )

/**
 * PumpCoreUnitTestCase001 repeatedly init-cleanup.
 */
PTEST_C_CASE_DEF(PumpCoreUnitTestCase001, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase001 repeatedly init-cleanup");
    {
        CApplication app;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
    }
    {
        CApplication app;
        CApplication app2;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
        PTEST_ASSERT((app2.IsInit() == PUMP_TRUE), "IsInit failed 1");
    }
    return PUMP_OK;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase002, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase002 init logger");
    // uninit call API
    PTEST_ASSERT((PUMP_CORE_LoggerCreate() == PUMP_NULL), "PUMP_CORE_LoggerCreate failed 1");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(PUMP_NULL, NULL) == PUMP_ERROR), "PUMP_CORE_LoggerConfig failed 1");
    // init call API
    {
        CApplication app;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.bPrintConsole = PUMP_TRUE;
        struLogCong.bWriteFile = PUMP_TRUE;
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        strcpy(struLogCong.szFilePath, "yz_log");
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate();
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 1");
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 2");
        __PUMP_CORE_INFO("-------PUMP_CORE_INFO bad1-------");
        __PUMP_CORE_WARING("-------PUMP_CORE_WARING  bad1-------");
        __PUMP_CORE_ERR("-------PUMP_CORE_ERR  bad1-------");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
        __PUMP_CORE_INFO("-------PUMP_CORE_INFO good-------");
        __PUMP_CORE_WARING("-------PUMP_CORE_WARING good-------");
        __PUMP_CORE_ERR("-------PUMP_CORE_ERR good-------");
    }
    // cleanup call API
    __PUMP_CORE_INFO("-------PUMP_CORE_INFO bad2-------");
    __PUMP_CORE_WARING("-------PUMP_CORE_WARING bad2-------");
    __PUMP_CORE_ERR("-------PUMP_CORE_ERR bad2-------");
    return  PUMP_OK;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase003, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase003 test CMutex");
    CMutex locker;
    locker.Lock();
    locker.Unlock();
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase004, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase004 test atomic option");
    int a;
    CMutex locker;
    CAtomicOp<CMutex, int> aOp(locker, a);
    ++aOp;
    CAtomicOpGuilder<int> aOpGuilder(a);
    ++aOpGuilder;
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase005, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase005 test cmdline parse");
    CCmdParser cmdLine;
    cmdLine.Add<std::string>("host", 'h', "host name", true, "");
    cmdLine.Add<int>("port", 'p', "port number", false, 80, Range(1, 65535));
    cmdLine.Add<std::string>("type", 't', "protocol type", false, "http", OneOfEnum<std::string>("http", "https", "ssh", "ftp"));
    cmdLine.Add("gzip", '\0', "gzip when transfer");
   char* argv_0[] = { "test.exe", "-h", "10.21.94.110", "-p", "8000", "-t", "http" };
    try
    {
        cmdLine.ParseCheck(sizeof(argv_0) / sizeof(void*), argv_0);
    }
    catch (std::exception & e)
    {
        PTEST_LOG(err, "%s", e.what());
        return PUMP_ERROR;
    }
    PTEST_LOG(msg, "%s%d", (cmdLine.Get<std::string>("type") + "://" + cmdLine.Get<std::string>("host") + ":").c_str(), cmdLine.Get<int>("port"));
    if (cmdLine.Exist("gzip"))
        PTEST_LOG(err, "gzip");

    char* argv_1[] = { "test.exe", "--host", "10.21.94.110", "--port", "8000", "-t", "type", "--help" };
    try
    {
        cmdLine.ParseCheck(sizeof(argv_1) / sizeof(void*), argv_1);
    }
    catch (std::exception & e)
    {
        PTEST_LOG(msg, "%s", e.what());
    }
    PTEST_LOG(msg, "%s%d", (cmdLine.Get<std::string>("type") + "://" + cmdLine.Get<std::string>("host") + ":").c_str(), cmdLine.Get<int>("port"));
    if (cmdLine.Exist("gzip"))
        PTEST_LOG(err, "gzip");
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase006, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase006 test environment var get/set");
    CEnvironment env;
    PTEST_ASSERT(env.GetEnvStrings()==PUMP_TRUE, "GetEnvStrings failed.");

    pump_char_t * pszString = (pump_char_t *)env.GetFirstString();
    while (pszString != NULL)
    {
        PTEST_LOG(msg, "$%s", pszString);
        pszString = (pump_char_t *)env.GetNextString();
    }
    PTEST_ASSERT(env.SetEnvVariable("MyEnv", "100") == PUMP_TRUE, "SetEnvStrings failed.");
    pump_char_t szBuf[260] = { 0 };
    PTEST_ASSERT(env.GetEnvVariable("MyEnv", szBuf, 260) == PUMP_TRUE, "GetEnvVariable MyEnv failed.");
    PTEST_ASSERT(strcmp(szBuf, "100") == 1, "test_cmd_env failed.");
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase007, PumpCoreUnitTestScene001,)
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase007 test environment data set");
    CEnvironment env;
    CEnvVarSet envSetDefault, envSetModify;
    env.GetForEnvVarSet(envSetDefault);
    PTEST_LOG(msg, ">>>>> Default EnvVar <<<<<<");
    for (CEnvVarSet::ConstIteratorType it = envSetDefault.CBegin(); it != envSetDefault.CEnd(); ++it)
    {
        PTEST_LOG(msg, "%s=", it->first.c_str(), envSetDefault.ToString(it->first).c_str());
    }
    // change env var
    envSetModify = envSetDefault;
    envSetModify.Insert("SSS", "AAAAAAA");
    env.SetFromEnvVarSet(envSetModify);
    env.GetForEnvVarSet(envSetModify);
    PTEST_LOG(msg, ">>>>> Modified EnvVar <<<<<<");
    for (CEnvVarSet::ConstIteratorType it = envSetModify.CBegin(); it != envSetModify.CEnd(); ++it)
    {
        PTEST_LOG(msg, "%s=", it->first.c_str(), envSetModify.ToString(it->first).c_str());
    }
    CEnvVarSet::IteratorType itSSS = envSetModify.Find("SSS");
    PTEST_ASSERT((itSSS == envSetModify.End()
        || itSSS->second.size() != 1
        || (*itSSS->second.begin()) != "AAAAAAA"
        || envSetModify.Size() != envSetDefault.Size() + 1) != PUMP_TRUE, "test_cmd_env_data_set failed");
    return 0;
}

class CLogTestThread : public CThread
{
public:
    virtual pump_int32_t Stop()
    {
        m_bStop = PUMP_TRUE;
        return 0;
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_bStop = PUMP_FALSE;
        while (!m_bStop)
        {
            __PUMP_CORE_INFO("-------PUMP_CORE_INFO %d-------", PUMP_CORE_Thread_GetSelfId());
            PUMP_CORE_Sleep(300);
        }
        return 0;
    }
private:
    pump_bool_t m_bStop;
};

#ifdef PUMP_CORE_HAVA_GLOG
PTEST_C_CASE_DEF(PumpCoreUnitTestCase008, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase008 test multi thread log <glog>");
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 3");
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.bPrintConsole = PUMP_TRUE;
    struLogCong.bWriteFile = PUMP_TRUE;
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    strcpy(struLogCong.szFilePath, "yz_log_glog");
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    pump_handle_t hLog = PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_GLOG);
    PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
    PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
    std::vector<CLogTestThread*> vecThx;
    for (int i = 0; i < 2;++i)
    {
        CLogTestThread * pthx = new CLogTestThread();
        pthx->Start();
        vecThx.push_back(pthx);
    }
    PUMP_CORE_Sleep(3000);
    for (size_t i = 0; i<vecThx.size();++i)
    {
        vecThx[i]->Stop();
        delete vecThx[i];
    }
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 1");
    return 0;
}
#endif // PUMP_CORE_HAVA_GLOG

PTEST_C_CASE_DEF(PumpCoreUnitTestCase009, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase009 test multi thread log <text>");
    {
        CApplication app;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.bPrintConsole = PUMP_TRUE;
        struLogCong.bWriteFile = PUMP_TRUE;
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        strcpy(struLogCong.szFilePath, "yz_log_text");
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate();
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
        std::vector<CLogTestThread*> vecThx;
        for (int i = 0; i < 2; ++i)
        {
            CLogTestThread * pthx = new CLogTestThread();
            pthx->Start();
            vecThx.push_back(pthx);
        }
        PUMP_CORE_Sleep(3000);
        for (size_t i = 0; i < vecThx.size(); ++i)
        {
            vecThx[i]->Stop();
            delete vecThx[i];
        }
    }
    return 0;
}

typedef void(*fnTEST_API_Function1)();
typedef void(*fnTEST_API_Function2)();

PTEST_C_CASE_DEF(PumpCoreUnitTestCase010, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase010 test load dll");
    pump_module_t hModule = PUMP_CORE_LoadDSo("E:/VMware/YZ/Pre-Research/dyanamic_load_dll/Debug/entry.dll");
    PTEST_ASSERT((hModule != PUMP_INVALID_MODULE), "load failed");
    fnTEST_API_Function1 pTEST_API_Function1 = (fnTEST_API_Function1)PUMP_CORE_GetDsoSym(hModule, "TEST_API_Function1");
    fnTEST_API_Function2 pTEST_API_Function2 = (fnTEST_API_Function2)PUMP_CORE_GetDsoSym(hModule, "TEST_API_Function2");
    PTEST_ASSERT(pTEST_API_Function1, "load TEST_API_Function1 failed");
    pTEST_API_Function1();
    PTEST_ASSERT(pTEST_API_Function2, "load pTEST_API_Function2 failed");
    pTEST_API_Function2();
    return 0;
}

pump_pvoid_t PUMP_CALLBACK My_ThreadPool_WorkRoutine(pump_pvoid_t pData)
{
    __PUMP_CORE_INFO("My_ThreadPool_WorkRoutine in", (int)pData);
    return NULL;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase011, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase011 test thread pool");
    {
        CApplication app;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.bPrintConsole = PUMP_TRUE;
        struLogCong.bWriteFile = PUMP_TRUE;
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        strcpy(struLogCong.szFilePath, "yz_log_text");
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate();
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");

        PUMP_CORE_InitThreadPool();
        pump_handle_t hThxPool = PUMP_CORE_ThreadPool_Create(2, 10);
        PTEST_ASSERT((hThxPool != PUMP_INVALID_THREADPOOL), "PUMP_CORE_ThreadPool_Create failed");
        for (int i = 0; i < 10; i++)
        {
            PTEST_ASSERT((PUMP_CORE_ThreadPool_Work(hThxPool, My_ThreadPool_WorkRoutine, (void*)i) != PUMP_ERROR)
                , "PUMP_CORE_ThreadPool_Work failed");
        }
        PTEST_ASSERT(PUMP_CORE_ThreadPool_Destroy(hThxPool) == PUMP_OK, "PUMP_CORE_ThreadPool_Destroy failed");
    }
    return 0;
}

void Test_CB_WriteLog (
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen)
{
    printf("[TestLog] %s\n", szMessage);
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase012, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase012 test user log api");
    {
        CApplication app;
        PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "IsInit failed 1");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.bPrintConsole = PUMP_TRUE;
        struLogCong.bWriteFile = PUMP_TRUE;
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        strcpy(struLogCong.szFilePath, "yz_log_text");
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_USER);
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
        struLogCong.pfnLog = Test_CB_WriteLog;
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");

        PUMP_CORE_INFO("=_=");
        PUMP_CORE_INFO("T_T");

    }
    return 0;
}

class CTestThreadInitCleanup
    : public ::Pump::Core::Thread::CThread
{
public:
    virtual pump_int32_t Stop()
    {
        m_bStop = PUMP_TRUE;
        return::Pump::Core::Thread::CThread::Stop();
    }
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        while (!m_bStop)
        {
            CApplication app;
        }
        return 0;
    }
private:
    pump_bool_t m_bStop = FALSE;
};

PTEST_C_CASE_DEF(PumpCoreUnitTestCase013, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase013 test muti-thread init cleanup");
    std::vector<CTestThreadInitCleanup *> vecThread;
    int nTestBatch = 50;
    for (int i = 0; i < nTestBatch; ++i)
    {
        CTestThreadInitCleanup * pthx = new CTestThreadInitCleanup();
        pthx->Start();
        vecThread.push_back(pthx);
    }
    PUMP_CORE_Sleep(10000);
    for (int i = 0; i < nTestBatch; ++i)
    {
        vecThread[i]->Stop();
        delete vecThread[i];
    }
    return 0;
}

class CTestPrivate
    : public CObjectPrivateBase
{
public:
    CTestPrivate(int a)
        : CObjectPrivateBase()
        , m_a(a)
    {

    }
    ~CTestPrivate()
    {

    }
    int GetA() const
    {
        return m_a;
    }
private:
    int m_a;
};

class CTestObject
    : public CObjectBase
{
public:
    CTestObject(int a)
        : CObjectBase("CObjectBase", PUMP_FALSE, new (PUMP_CORE_ALLOC_MEMPOLL)CTestPrivate(a))
    {

    }
    ~CTestObject()
    {
        delete m_pPrimitive;
    }
    PUMP_CORE_ALLOC_TYPE GetPrivAllocType() const
    {
        return m_pPrimitive->GetAllocType();
    }
    size_t GetPrivSize() const
    {
        return m_pPrimitive->GetSize();
    }
    int GetPrivA() const
    {
        CTestPrivate * m_priv = (CTestPrivate *)m_pPrimitive;
        return m_priv->GetA();
    }
};

PTEST_C_CASE_DEF(PumpCoreUnitTestCase014, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase014 test public object");
    std::vector<CTestObject *> vecObject;
    int nTestBatch = 50;
    for (int i = 0; i < nTestBatch; ++i)
    {
        CTestObject * pobj = new CTestObject(i);
        vecObject.push_back(pobj);
    }
    for (int i = 0; i < nTestBatch; ++i)
    {
        PTEST_ASSERT((vecObject[i]->GetSize() == sizeof(CTestObject)), "size not match 1");
        PTEST_ASSERT((vecObject[i]->GetPrivSize() == sizeof(CTestPrivate)), "priv size not match 1");
        PTEST_ASSERT((vecObject[i]->GetAllocType() == PUMP_CORE_ALLOC_BUILDIN), "AllocType not match 1");
        PTEST_ASSERT((vecObject[i]->GetPrivAllocType() == PUMP_CORE_ALLOC_MEMPOLL), "AllocType not match 1");
        PTEST_ASSERT((vecObject[i]->GetPrivA() == i), "A not match 1");
        delete vecObject[i];
    }
    vecObject.clear();
    for (int i = 0; i < nTestBatch; ++i)
    {
        CTestObject * pobj = new (PUMP_CORE_ALLOC_MEMPOLL)CTestObject(i);
        vecObject.push_back(pobj);
    }
    for (int i = 0; i < nTestBatch; ++i)
    {
        PTEST_ASSERT((vecObject[i]->GetSize() == sizeof(CTestObject)), "size not match 2");
        PTEST_ASSERT((vecObject[i]->GetPrivSize() == sizeof(CTestPrivate)), "priv size not match 2");
        PTEST_ASSERT((vecObject[i]->GetAllocType() == PUMP_CORE_ALLOC_MEMPOLL), "AllocType not match 2");
        PTEST_ASSERT((vecObject[i]->GetPrivAllocType() == PUMP_CORE_ALLOC_MEMPOLL), "AllocType not match 2");
        PTEST_ASSERT((vecObject[i]->GetPrivA() == i), "A not match 2");
        delete vecObject[i];
    }
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase015, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase015 test CStringBuffer");
    CStringBuffer buff0;
    CStringBuffer buff1(128, 1024);
    PTEST_ASSERT((buff0.GetBufferSize() == 0), "size not match 1");
    PTEST_ASSERT((buff0.GetBufferRollSize() == 128), "roll not match 1");
    PTEST_ASSERT((buff0.GetBufferCapacity() == 0), "cap not match 1");
    PTEST_ASSERT((buff1.GetBufferSize() == 0), "size not match 2");
    PTEST_ASSERT((buff1.GetBufferRollSize() == 128), "roll not match 2");
    PTEST_ASSERT((buff1.GetBufferCapacity() == 1024), "cap not match 2");

    CStringBuffer buff2(buff1);
    PTEST_ASSERT((buff2.GetBufferSize() == 0), "size not match 3");
    PTEST_ASSERT((buff2.GetBufferRollSize() == 128), "roll not match 3");
    PTEST_ASSERT((buff2.GetBufferCapacity() == 1024), "cap not match 3");
    PTEST_ASSERT((buff2.GetCPtr() != buff1.GetCPtr()), "ptr match 3");
    return 0;
}

PTEST_C_CASE_DEF(PumpCoreUnitTestCase016, PumpCoreUnitTestScene001, )
{
    PTEST_LOG(comment, "PumpCoreUnitTestCase016 test CStringBuffer");
    PTEST_LOG(log, "%s", PUMP_CORE_VERSION);
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
    return getchar();
PTEST_MAIN_END