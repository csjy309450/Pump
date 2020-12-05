//
// Created by yz on 19-10-13.
//

#include <cstdio>
#include <cstring>
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/thread/pump_core_atomic_op.hpp"
#include "pump_core/network/pump_core_sock.h"
#include "pump_core/pump_core_cmdparse.hpp"
#include "pump_core/pump_core_environment.h"
#include "pump_test/pump_test.h"
#include "pump_core/logger/__pump_core_inner_logger.h"

using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Core::Net;
using namespace Pump::Core::Thread;
using namespace Pump::Core::Cmder;
using namespace Pump::Test;

PTEST_C_SCENE_DEF(UnitTestScene000, )

/**
 * UnitTestCase001 repeatedly init-cleanup.
 */
PTEST_C_CASE_DEF(UnitTestCase001, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase001 repeatedly init-cleanup");
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 1");
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 1.1");
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 1");
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 2");
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 2");
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 3");
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 3");
    return PUMP_OK;
}

PTEST_C_CASE_DEF(UnitTestCase002, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase002 init logger");
    // uninit call API
    PTEST_ASSERT((PUMP_CORE_LoggerCreate() == PUMP_NULL), "PUMP_CORE_LoggerCreate failed 1");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(PUMP_NULL, NULL) == PUMP_ERROR), "PUMP_CORE_LoggerConfig failed 1");
    // init call API
    PTEST_ASSERT((PUMP_CORE_Init() == PUMP_OK), "PUMP_CORE_Init failed 1");
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.szFilePath = "";
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    pump_handle_t hLog = PUMP_CORE_LoggerCreate();
    PTEST_ASSERT(hLog!=PUMP_NULL, "PUMP_CORE_LoggerCreate failed 1");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong)==PUMP_OK), "PUMP_CORE_LoggerConfig failed 2");
    __PUMP_CORE_INFO << "-------PUMP_CORE_INFO bad1-------";
    __PUMP_CORE_WARING << "-------PUMP_CORE_WARING  bad1-------";
    __PUMP_CORE_ERR << "-------PUMP_CORE_ERR  bad1-------";
    PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog)==PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
    __PUMP_CORE_INFO << "-------PUMP_CORE_INFO good-------";
    __PUMP_CORE_WARING << "-------PUMP_CORE_WARING good-------";
    __PUMP_CORE_ERR << "-------PUMP_CORE_ERR good-------";
    PTEST_ASSERT((PUMP_CORE_Cleanup() == PUMP_OK), "PUMP_CORE_Cleanup failed 1");
    // cleanup call API
    __PUMP_CORE_INFO << "-------PUMP_CORE_INFO bad2-------";
    __PUMP_CORE_WARING << "-------PUMP_CORE_WARING bad2-------";
    __PUMP_CORE_ERR << "-------PUMP_CORE_ERR bad2-------";
    return  PUMP_OK;
}

PTEST_C_CASE_DEF(UnitTestCase003, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase003 test CMutex");
    CMutex locker;
    locker.Lock();
    locker.Unlock();
    return 0;
}

PTEST_C_CASE_DEF(UnitTestCase004, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase004 test atomic option");
    int a;
    CMutex locker;
    CAtomicOp<CMutex, int> aOp(locker, a);
    ++aOp;
    CAtomicOpGuilder<int> aOpGuilder(a);
    ++aOpGuilder;
    return 0;
}

PTEST_C_CASE_DEF(UnitTestCase005, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase005 test cmdline parse");
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

PTEST_C_CASE_DEF(UnitTestCase006, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase006 test environment var get/set");
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

PTEST_C_CASE_DEF(UnitTestCase007, UnitTestScene000,)
{
    PTEST_LOG(comment, "UnitTestCase007 test environment data set");
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

PTEST_MAIN_BEGINE(int argc, char** argv)
    //test_logger();
    //__PUMP_CORE_Test_new_logger();
    /*test_mutex();
    test_atomicOp();*/
    //test_cmd_parse();
    //test_cmd_env();
    //test_cmd_env_data_set();
    return getchar();
PTEST_MAIN_END