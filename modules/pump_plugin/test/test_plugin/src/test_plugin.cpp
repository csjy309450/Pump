// dyanamic_load_dll.cpp : 定义控制台应用程序的入口点。
//

#include "pump_core/pump_core_api.h"
#include "pump_core/pump_core_string.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_plugin/pump_plugin_config.h"
#define PUMP_PPLUG_BUILD_DLL 1
#include "pump_plugin/pump_pplug.h"
#include "pump_plugin/pump_plugin_log.h"

PUMP_PPLUG_DEF(test_plugin,9,0,0)

class CMemoryLeackTest
{
public:
    CMemoryLeackTest()
    { 
        PUMP_PLUGIN_INFO("ctest"); 
        for (int i = 0; i < 256; i++)
        {
            s[i] = new char[1024*1024];
        }
    }
    ~CMemoryLeackTest()
    { 
        PUMP_PLUGIN_INFO("~ctest"); 
        for (int i = 0; i < 256; i++)
        {
            delete[] s[i];
        }
    }
    char *s[256];
};

class CMainThread 
    : public ::Pump::Core::Thread::CThread
{
public:
    virtual pump_int32_t Stop()
    {
        m_bStop = PUMP_TRUE;
        return ::Pump::Core::Thread::CThread::Stop();
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        PUMP_PLUGIN_INFO(">>> loop in");
        m_bStop = PUMP_FALSE;
        while (!m_bStop)
        {
            PUMP_PLUGIN_INFO("one loop");
            PUMP_CORE_Sleep(1000);
        }
        PUMP_PLUGIN_INFO(">>> loop out");
        return 0;
    }
private:
    pump_bool_t m_bStop;
};

static CMainThread thxMain;
static CMemoryLeackTest * ptest = NULL;

static int PUMP_PPLUG_FNAME(test_plugin, Init)()
{
    PUMP_PLUGIN_INFO("%s in", PUMP_PLUGIN_TOSTR(PUMP_PPLUG_FNAME(test_plugin, Init)));
    PUMP_CORE_Init(); 
    ptest = new CMemoryLeackTest();
    return 0;
}

static int PUMP_PPLUG_FNAME(test_plugin, Cleanup)()
{
    PUMP_PLUGIN_INFO("%s in", PUMP_PLUGIN_TOSTR(PUMP_PPLUG_FNAME(test_plugin, Cleanup)));
    thxMain.Stop();
    PUMP_CORE_Cleanup();
    delete ptest;
    return 0;
}

static int PUMP_PPLUG_FNAME(test_plugin, Start)()
{
    PUMP_PLUGIN_INFO("%s in", PUMP_PLUGIN_TOSTR(PUMP_PPLUG_FNAME(test_plugin, Start)));
    thxMain.Start();
    return 0;
}

static int PUMP_PPLUG_FNAME(test_plugin, Request)(const PUMP_PPLUG_REQUEST * req)
{
    PUMP_PLUGIN_INFO("%s in", PUMP_PLUGIN_TOSTR(PUMP_PPLUG_FNAME(test_plugin, Request)));
    if (strcmp(req->req_msg,"stop")==0)
    {
        thxMain.Stop();
    }
    return 0;
}
