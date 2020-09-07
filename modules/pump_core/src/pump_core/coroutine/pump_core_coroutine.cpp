/**
 * @file pump_core_coroutine.cpp
 * @brief Library Header.
 * @details Declaration of coroutine interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-09-07
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-09-07<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/coroutine/pump_core_coroutine.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// 纤程栈大小
#define _INT_STACK        (1024 * 1024)
// 默认初始化创建纤程数目
#define _INT_COROUTINE    (16)

/*
* 单个纤程单元 coroutine , 还有纤程集管理器 comng
*/
struct __PUMP_COROUTINE;

#if defined(PUMP_OS_POSIX)

#include <ucontext.h>
#include <stddef.h>
#include <stdint.h>

struct _PUMP_CO_CONTEX {
    char stack[_INT_STACK];
    ucontext_t main;            // 纤程管理器中保存的临时纤程对象
    int running;                // 当前纤程管理器中运行的纤程id
    int nco;                    // 当前纤程集轮询中当前索引
    int cap;                    // 纤程集容量,
    struct __PUMP_COROUTINE ** co;        // 保存的纤程集
};

struct __PUMP_COROUTINE {
    char * stack;
    ucontext_t ctx;                // 操作系统纤程对象
    ptrdiff_t cap;
    ptrdiff_t size;
    PUMP_CORE_CoPCallBack func;                    // 纤程执行的函数体
    void * ud;                    // 纤程执行的额外参数
    PUMP_COSTATUS status;            // 当前纤程运行状态
    struct _PUMP_CO_CONTEX * comng;        // 当前纤程集管理器
};

/*
* 开启纤程系统, 并创建主纤程
*            : 返回开启的纤程调度系统管理器
*/
PUMP_CORE_API pump_co_t PUMP_CALLBACK PUMP_CORE_CoOpen(void) 
{
    struct _PUMP_CO_CONTEX * comng = malloc(sizeof(struct _PUMP_CO_CONTEX));
    assert(NULL != comng);
    comng->nco = 0;
    comng->running = -1;
    comng->co = calloc(comng->cap = _INT_COROUTINE, sizeof(struct __PUMP_COROUTINE *));
    assert(NULL != comng->co);
    return comng;
}

// 销毁一个纤程
PUMP_CORE_API void PUMP_CALLBACK __PUMP_CORE_INNER_CoDelete(struct __PUMP_COROUTINE * co) 
{
    free(co->stack);
    free(co);
}

// 创建一个纤程对象
static PUMP_INLINE struct __PUMP_COROUTINE * __PUMP_CORE_INNER_CoNew(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud) 
{
    struct __PUMP_COROUTINE * co = malloc(sizeof(struct __PUMP_COROUTINE));
    assert(co && comng && func);
    co->func = func;
    co->ud = ud;
    co->comng = comng;
    co->status = PUMP_COSTATUS_READY;
    co->cap = 0;
    co->size = 0;
    co->stack = NULL;
    return co;
}

static PUMP_INLINE void __PUMP_CORE_INNER_CoMain(uint32_t low32, uint32_t hig32) 
{
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hig32 << 32);
    struct _PUMP_CO_CONTEX * comng = (struct _PUMP_CO_CONTEX *)ptr;
    int id = comng->running;
    struct __PUMP_COROUTINE * co = comng->co[id];
    co->func(comng, co->ud);
    __PUMP_CORE_INNER_CoDelete(co);
    comng->co[id] = NULL;
    --comng->nco;
    comng->running = -1;
}

/*
* 激活创建的纤程对象.
* comng    : 纤程管理器对象
* id        : PUMP_CORE_CoCreate 创建的纤程对象
*/
PUMP_CALLBACK void PUMP_CALLBACK PUMP_CORE_CoResume(pump_co_t comng, int id) 
{
    struct __PUMP_COROUTINE * co;
    uintptr_t ptr;
    assert(comng->running == -1 && id >= 0 && id < comng->cap);
    co = comng->co[id];
    if (NULL == co || co->status == PUMP_COSTATUS_DEAD)
        return;
    switch (co->status) {
    case PUMP_COSTATUS_READY:
        comng->running = id;
        co->status = PUMP_COSTATUS_RUNNING;
        getcontext(&co->ctx);
        co->ctx.uc_stack.ss_sp = comng->stack;
        co->ctx.uc_stack.ss_size = _INT_STACK;
        co->ctx.uc_link = &comng->main;
        ptr = (uintptr_t)comng;
        makecontext(&co->ctx, (void(*)())__PUMP_CORE_INNER_CoMain, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));
        swapcontext(&comng->main, &co->ctx);
        break;
    case PUMP_COSTATUS_SUSPEND:
        comng->running = id;
        co->status = PUMP_COSTATUS_RUNNING;
        // stack add is high -> low
        memcpy(comng->stack + _INT_STACK - co->size, co->stack, co->size);
        swapcontext(&comng->main, &co->ctx);
        break;
    default:
        assert(0);
    }
}

// 保存当前运行的堆栈信息
static void __PUMP_CORE_INNER_SaveStack(struct __PUMP_COROUTINE * co, char * top) 
{
    char dummy = 0;
    assert(top - &dummy <= _INT_STACK);
    if (co->cap < top - &dummy) {
        free(co->stack);
        co->cap = top - &dummy;
        co->stack = malloc(co->cap);
        assert(co->stack);
    }
    co->size = top - &dummy;
    memcpy(co->stack, &dummy, co->size);
}

/*
* 中断当前运行的的纤程, 并将CPU交给主纤程处理调度.
* comng    : 纤程管理器对象
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoYield(pump_co_t comng) 
{
    struct __PUMP_COROUTINE * co;
    int id = comng->running;
    assert(id >= 0);
    co = comng->co[id];
    assert((char *)&co > comng->stack);
    __PUMP_CORE_INNER_SaveStack(co, comng->stack + _INT_STACK);
    co->status = PUMP_COSTATUS_SUSPEND;
    comng->running = -1;
    swapcontext(&co->ctx, &comng->main);
}

#elif defined(PUMP_OS_WINDOWS)

struct _PUMP_CO_CONTEX {
    PVOID main;                    // 纤程管理器中保存的临时纤程对象
    int running;                // 当前纤程管理器中运行的纤程id
    int nco;                    // 当前纤程集轮询中当前索引
    int cap;                    // 纤程集容量,
    struct __PUMP_COROUTINE ** co;        // 保存的纤程集
};

struct __PUMP_COROUTINE {
    PVOID ctx;                    // 操作系统纤程对象                
    PUMP_CORE_CoPCallBack func;                    // 纤程执行的函数体
    void * ud;                    // 纤程执行的额外参数
    PUMP_COSTATUS status;            // 当前纤程运行状态
    struct _PUMP_CO_CONTEX * comng;        // 当前纤程集管理器
};

/*
* 开启纤程系统, 并创建主纤程
*            : 返回开启的纤程调度系统管理器
*/
PUMP_CORE_API pump_co_t PUMP_CALLBACK PUMP_CORE_CoOpen(void) 
{
    struct _PUMP_CO_CONTEX * comng = (struct _PUMP_CO_CONTEX *)malloc(sizeof(struct _PUMP_CO_CONTEX));
    assert(NULL != comng);
    comng->nco = 0;
    comng->running = -1;
    comng->co = (struct __PUMP_COROUTINE **)calloc(comng->cap = _INT_COROUTINE, sizeof(struct __PUMP_COROUTINE *));
    assert(NULL != comng->co);
    // 开启Window协程
    comng->main = ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
    return comng;
}

// 销毁一个纤程
static PUMP_INLINE void __PUMP_CORE_INNER_CoDelete(struct __PUMP_COROUTINE * co) 
{
    DeleteFiber(co->ctx);
    free(co);
}

// 创建一个纤程对象
static PUMP_INLINE struct __PUMP_COROUTINE * __PUMP_CORE_INNER_CoNew(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud) 
{
    struct __PUMP_COROUTINE * co = (struct __PUMP_COROUTINE *)malloc(sizeof(struct __PUMP_COROUTINE));
    assert(co && comng && func);
    co->func = func;
    co->ud = ud;
    co->comng = comng;
    co->status = PUMP_COSTATUS_READY;
    return co;
}

static PUMP_INLINE VOID WINAPI __PUMP_CORE_INNER_CoMain(LPVOID ptr) 
{
    struct _PUMP_CO_CONTEX * comng = (struct _PUMP_CO_CONTEX *)ptr;
    int id = comng->running;
    struct __PUMP_COROUTINE * co = comng->co[id];
    co->func(comng, co->ud);
    __PUMP_CORE_INNER_CoDelete(co);
    comng->co[id] = NULL;
    --comng->nco;
    comng->running = -1;
}

/*
* 激活创建的纤程对象.
* comng    : 纤程管理器对象
* id        : PUMP_CORE_CoCreate 创建的纤程对象
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoResume(pump_co_t comng, int id) 
{
    struct __PUMP_COROUTINE * co;
    assert(comng->running == -1 && id >= 0 && id < comng->cap);
    co = comng->co[id];
    if (NULL == co || co->status == PUMP_COSTATUS_DEAD)
        return;
    switch (co->status) {
    case PUMP_COSTATUS_READY:
        comng->running = id;
        co->status = PUMP_COSTATUS_RUNNING;
        co->ctx = CreateFiberEx(_INT_STACK, 0, FIBER_FLAG_FLOAT_SWITCH, __PUMP_CORE_INNER_CoMain, comng);
        comng->main = GetCurrentFiber();
        SwitchToFiber(co->ctx);
        break;
    case PUMP_COSTATUS_SUSPEND:
        comng->running = id;
        co->status = PUMP_COSTATUS_RUNNING;
        comng->main = GetCurrentFiber();
        SwitchToFiber(co->ctx);
        break;
    default:
        assert(0);
    }
}

/*
* 中断当前运行的的纤程, 并将CPU交给主纤程处理调度.
* comng    : 纤程管理器对象
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoYield(pump_co_t comng) 
{
    struct __PUMP_COROUTINE * co;
    int id = comng->running;
    assert(id >= 0);
    co = comng->co[id];
    co->status = PUMP_COSTATUS_SUSPEND;
    comng->running = -1;
    co->ctx = GetCurrentFiber();
    SwitchToFiber(comng->main);
}

#endif

/*
* 关闭开启的纤程系统
* comng    : PUMP_CORE_CoOpen 返回的纤程管理器
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoClose(pump_co_t comng) 
{
    int i;
    for (i = 0; i < comng->cap; ++i) {
        struct __PUMP_COROUTINE * co = comng->co[i];
        if (co) {
            __PUMP_CORE_INNER_CoDelete(co);
            comng->co[i] = NULL;
        }
    }
    free(comng->co);
    comng->co = NULL;
    free(comng);
}

/*
* 创建一个纤程对象,并返回创建纤程的id. 创建好的纤程状态是PUMP_COSTATUS_READY
* comng    : PUMP_CORE_CoOpen 返回的纤程管理器
* func        : 纤程运行的主体
* ud        : 用户传入的数据, PUMP_CORE_CoPCallBack 中 ud 会使用
*            : 返回创建好的纤程标识id
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoCreate(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud) 
{
    struct __PUMP_COROUTINE * co = __PUMP_CORE_INNER_CoNew(comng, func, ud);
    // 下面是普通情况, 可以找见
    if (comng->nco < comng->cap) {
        int i;
        for (i = 0; i < comng->cap; ++i) {
            int id = (i + comng->nco) % comng->cap;
            if (NULL == comng->co[id]) {
                comng->co[id] = co;
                ++comng->nco;
                return id;
            }
        }
        assert(i == comng->cap);
        return -1;
    }

    // 需要重新分配空间, 构造完毕后返回
    comng->co = (struct __PUMP_COROUTINE **)realloc(comng->co, sizeof(struct __PUMP_COROUTINE *) * comng->cap * 2);
    assert(NULL != comng->co);
    memset(comng->co + comng->cap, 0, sizeof(struct __PUMP_COROUTINE *) * comng->cap);
    comng->cap <<= 1;
    comng->co[comng->nco] = co;
    return comng->nco++;
}

/*
* 得到当前纤程运行的状态
* comng    : 纤程管理器对象
* id        : PUMP_CORE_CoCreate 创建的纤程对象
*            : 返回状态具体参照 PUMP_COSTATUS
*/
PUMP_CORE_API PUMP_COSTATUS PUMP_CALLBACK PUMP_CORE_CoGetStatus(pump_co_t comng, int id) 
{
    assert(comng && id >= 0 && id < comng->cap);
    return comng->co[id] ? comng->co[id]->status : PUMP_COSTATUS_DEAD;
}

/*
* 得到当前纤程系统中运行的纤程, 返回 < 0表示没有纤程在运行
* comng    : 纤程管理器对象
*            : 返回当前运行的纤程标识id,
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoGetRunning(pump_co_t comng) 
{
    return comng->running;
}
