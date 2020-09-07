#include <stdio.h>
#include "pump_core/coroutine/pump_core_coroutine.h"

struct args {
    int n;
};

static void _foo(pump_co_t comng, void * ud) {
    struct args * arg = (struct args *)ud;
    int start = arg->n;
    int i;
    for (i = 0; i<5; i++) {
        printf("coroutine %d : %d\n", PUMP_CORE_CoGetRunning(comng), start + i);
        PUMP_CORE_CoYield(comng);
    }
}

static void _test(void * pcomng) {

    pump_co_t comng = (pump_co_t)pcomng;
    struct args arg1 = { 0 };
    struct args arg2 = { 100 };

    int co1 = PUMP_CORE_CoCreate(comng, _foo, &arg1);
    int co2 = PUMP_CORE_CoCreate(comng, _foo, &arg2);
    printf("main start\n");
    while (PUMP_CORE_CoGetStatus(comng, co1) && PUMP_CORE_CoGetStatus(comng, co2)) {
        PUMP_CORE_CoResume(comng, co1);
        PUMP_CORE_CoResume(comng, co2);
    }
    printf("main end\n");
}

/*
* test coroutine demo
*/
int main(int argc, char * argv[]) {
    pump_co_t comng = PUMP_CORE_CoOpen();
    _test(comng);
    PUMP_CORE_CoClose(comng);

    return getchar();
}