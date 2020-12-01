#include "pump_core/pump_core_config.h"
#include "pump_core/thread/pump_core_thread_pool.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"

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

pump_pvoid_t PUMP_CALLBACK My_ThreadPool_WorkRoutine(pump_pvoid_t pData)
{
    PUMP_CORE_INFO << "My_ThreadPool_WorkRoutine in" << (int)pData;
    return NULL;
}

int main(int argc, char**argv)
{
    test_init();
    test_logger();
    PUMP_CORE_InitThreadPool();
    pump_handle_t hThxPool = PUMP_CORE_ThreadPool_Create(2,10);
    if (hThxPool == PUMP_INVALID_THREADPOOL)
    {
        PUMP_CORE_ERR << "PUMP_CORE_ThreadPool_Create failed";
        return -1;
    }
    for (int i = 0; i < 10;i++)
    {
        PUMP_CORE_ThreadPool_Work(hThxPool, My_ThreadPool_WorkRoutine, (void*)i);
    }
    

    return getchar();
}