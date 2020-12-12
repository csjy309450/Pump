#include <iostream>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_plugin/pump_pslot.h"

int main(int argc, char** argv)
{
    PUMP_PSLOT_Init();
    pump_plugin_id id = PUMP_PSLOT_PluginLoad("test_plugin", "0.9.1");
    if (id == -1)
    {
        printf("load plugin failed\n");
        return getchar();
    }
    printf("load plugin succeed\n");
    return getchar();
}