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
//#include "pump_test/pump_test_def.h"

using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Core::Net;
using namespace Pump::Core::Thread;
using namespace Pump::Core::Cmder;

//template class PUMP_CORE_CLASS CAtomicOp<CMutex, int>;

int test_init()
{
    PUMP_CORE_Init();
    return 0;
}

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

int test_mutex()
{
    CMutex locker;
    locker.Lock();
    return 0;
}

int test_atomicOp()
{
    int a;
    CMutex locker;
    CAtomicOp<CMutex, int> aOp(locker, a);
    ++aOp;
    CAtomicOpGuilder<int> aOpGuilder(a);
    ++aOpGuilder;
    return 0;
}

int __cmd_parse(int argc, char** argv)
{
    CCmdParser a;
    a.Add<std::string>("host", 'h', "host name", true, "");
    a.Add<int>("port", 'p', "port number", false, 80, Range(1, 65535));
    a.Add<std::string>("type", 't', "protocol type", false, "http", OneOfEnum<std::string>("http", "https", "ssh", "ftp"));
    a.Add("gzip", '\0', "gzip when transfer");
    try
    {
        a.ParseCheck(argc, argv);
    }
    catch (std::exception & e)
    {
        PUMP_CORE_INFO <<'\n'<< e.what();
        return -1;
    }

    PUMP_CORE_INFO << '\n' << a.Get<std::string>("type") << "://"
        << a.Get<std::string>("host") << ":"
        << a.Get<int>("port");
    if (a.Exist("gzip"))
        PUMP_CORE_INFO << "gzip";
    return 0;
}

int test_cmd_parse()
{
    char* argv_0[] = { "test.exe", "-h", "10.21.94.110", "-p", "8000", "-t", "http" };
    __cmd_parse(sizeof(argv_0) / sizeof(void*), argv_0);

    char* argv_1[] = { "test.exe", "--host", "10.21.94.110", "--port", "8000", "-t", "type", "--help" };
    __cmd_parse(sizeof(argv_1) / sizeof(void*), argv_1);
    return 0;
}

int test_cmd_env()
{
    CEnvironment env;
    env.GetEnvStrings();

    pump_char_t * pszString = (pump_char_t *)env.GetFirstString();
    while (pszString != NULL)
    {
        PUMP_CORE_INFO << "$" << pszString;
        pszString = (pump_char_t *)env.GetNextString();
    }

    env.SetEnvVariable("MyEnv", "100");
    pump_char_t szBuf[260] = { 0 };
    env.GetEnvVariable("MyEnv", szBuf, 260);
    if (::strcmp(szBuf, "100"))
    {
        PUMP_CORE_ERR << ">>>>> test_cmd_env failed <<<<<<";
    }
    else
    {
        PUMP_CORE_INFO << ">>>>> test_cmd_env succed <<<<<<";
    }
    return 0;
}

int test_cmd_env_data_set()
{
    CEnvironment env;
    CEnvVarSet envSetDefault, envSetModify;
    env.GetForEnvVarSet(envSetDefault);
    PUMP_CORE_INFO << ">>>>> Default EnvVar <<<<<<";
    for (CEnvVarSet::ConstIteratorType it = envSetDefault.CBegin(); it != envSetDefault.CEnd(); ++it)
    {
        PUMP_CORE_INFO << it->first << "=" << envSetDefault.ToString(it->first);
    }
    // change env var
    envSetModify = envSetDefault;
    envSetModify.Insert("SSS", "AAAAAAA");
    env.SetFromEnvVarSet(envSetModify);
    env.GetForEnvVarSet(envSetModify);
    PUMP_CORE_INFO << ">>>>> Modified EnvVar <<<<<<";
    for (CEnvVarSet::ConstIteratorType it = envSetModify.CBegin(); it != envSetModify.CEnd(); ++it)
    {
        PUMP_CORE_INFO << it->first << "=" << envSetModify.ToString(it->first);
    }
    CEnvVarSet::IteratorType itSSS = envSetModify.Find("SSS");
    if (itSSS == envSetModify.End()
        || itSSS->second.size() != 1 
        || (*itSSS->second.begin())!="AAAAAAA"
        || envSetModify.Size() != envSetDefault.Size() + 1)
    {
        PUMP_CORE_ERR << ">>>>> test_cmd_env_data_set failed <<<<<<";
    }
    else
    {
        PUMP_CORE_INFO << ">>>>> test_cmd_env_data_set succed <<<<<<";
    }
    return 0;
}

int main(int argc, char** argv)
{
    test_init();
    //test_logger();
    //__PUMP_CORE_Test_new_logger();
    /*test_mutex();
    test_atomicOp();*/
    //test_cmd_parse();
    //test_cmd_env();
    //test_cmd_env_data_set();
    return getchar();
}