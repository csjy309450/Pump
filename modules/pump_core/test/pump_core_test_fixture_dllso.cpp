#include <iostream>
#include"pump_core/pump_core_dllso.h"

typedef void(*fnTEST_API_Function1)();
typedef void(*fnTEST_API_Function2)();

int main(int argc, char** argv)
{
    pump_module_t hModule = PUMP_CORE_LoadDSo("E:/VMware/YZ/Pump/vs2013/bin/win_x86/Debug/dyanamic_load_dll.dll");
    if (hModule==PUMP_INVALID_MODULE)
    {
        printf("load failed\n");
        goto ret_error;
    }
    fnTEST_API_Function1 pTEST_API_Function1 = (fnTEST_API_Function1)PUMP_CORE_GetDsoSym(hModule, "TEST_API_Function1");
    fnTEST_API_Function2 pTEST_API_Function2 = (fnTEST_API_Function2)PUMP_CORE_GetDsoSym(hModule, "TEST_API_Function2");
    if (!pTEST_API_Function1)
    {
        printf("load TEST_API_Function1 failed\n");
    }
    pTEST_API_Function1();
    if (!pTEST_API_Function2)
    {
        printf("load pTEST_API_Function2 failed\n");
    }
    pTEST_API_Function2();
ret_error:
    return getchar();
}