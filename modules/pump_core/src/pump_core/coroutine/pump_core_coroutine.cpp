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

// �˳�ջ��С
#define _INT_STACK        (1024 * 1024)
// Ĭ�ϳ�ʼ�������˳���Ŀ
#define _INT_COROUTINE    (16)

/*
* �����˳̵�Ԫ coroutine , �����˳̼������� comng
*/
struct __PUMP_COROUTINE;

#if defined(PUMP_OS_POSIX)

#include <ucontext.h>
#include <stddef.h>
#include <stdint.h>

struct _PUMP_CO_CONTEX {
    char stack[_INT_STACK];
    ucontext_t main;            // �˳̹������б������ʱ�˳̶���
    int running;                // ��ǰ�˳̹����������е��˳�id
    int nco;                    // ��ǰ�˳̼���ѯ�е�ǰ����
    int cap;                    // �˳̼�����,
    struct __PUMP_COROUTINE ** co;        // ������˳̼�
};

struct __PUMP_COROUTINE {
    char * stack;
    ucontext_t ctx;                // ����ϵͳ�˳̶���
    ptrdiff_t cap;
    ptrdiff_t size;
    PUMP_CORE_CoPCallBack func;                    // �˳�ִ�еĺ�����
    void * ud;                    // �˳�ִ�еĶ������
    PUMP_COSTATUS status;            // ��ǰ�˳�����״̬
    struct _PUMP_CO_CONTEX * comng;        // ��ǰ�˳̼�������
};

/*
* �����˳�ϵͳ, ���������˳�
*            : ���ؿ������˳̵���ϵͳ������
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

// ����һ���˳�
PUMP_CORE_API void PUMP_CALLBACK __PUMP_CORE_INNER_CoDelete(struct __PUMP_COROUTINE * co) 
{
    free(co->stack);
    free(co);
}

// ����һ���˳̶���
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
* ��������˳̶���.
* comng    : �˳̹���������
* id        : PUMP_CORE_CoCreate �������˳̶���
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

// ���浱ǰ���еĶ�ջ��Ϣ
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
* �жϵ�ǰ���еĵ��˳�, ����CPU�������˳̴������.
* comng    : �˳̹���������
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
    PVOID main;                    // �˳̹������б������ʱ�˳̶���
    int running;                // ��ǰ�˳̹����������е��˳�id
    int nco;                    // ��ǰ�˳̼���ѯ�е�ǰ����
    int cap;                    // �˳̼�����,
    struct __PUMP_COROUTINE ** co;        // ������˳̼�
};

struct __PUMP_COROUTINE {
    PVOID ctx;                    // ����ϵͳ�˳̶���                
    PUMP_CORE_CoPCallBack func;                    // �˳�ִ�еĺ�����
    void * ud;                    // �˳�ִ�еĶ������
    PUMP_COSTATUS status;            // ��ǰ�˳�����״̬
    struct _PUMP_CO_CONTEX * comng;        // ��ǰ�˳̼�������
};

/*
* �����˳�ϵͳ, ���������˳�
*            : ���ؿ������˳̵���ϵͳ������
*/
PUMP_CORE_API pump_co_t PUMP_CALLBACK PUMP_CORE_CoOpen(void) 
{
    struct _PUMP_CO_CONTEX * comng = (struct _PUMP_CO_CONTEX *)malloc(sizeof(struct _PUMP_CO_CONTEX));
    assert(NULL != comng);
    comng->nco = 0;
    comng->running = -1;
    comng->co = (struct __PUMP_COROUTINE **)calloc(comng->cap = _INT_COROUTINE, sizeof(struct __PUMP_COROUTINE *));
    assert(NULL != comng->co);
    // ����WindowЭ��
    comng->main = ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
    return comng;
}

// ����һ���˳�
static PUMP_INLINE void __PUMP_CORE_INNER_CoDelete(struct __PUMP_COROUTINE * co) 
{
    DeleteFiber(co->ctx);
    free(co);
}

// ����һ���˳̶���
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
* ��������˳̶���.
* comng    : �˳̹���������
* id        : PUMP_CORE_CoCreate �������˳̶���
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
* �жϵ�ǰ���еĵ��˳�, ����CPU�������˳̴������.
* comng    : �˳̹���������
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
* �رտ������˳�ϵͳ
* comng    : PUMP_CORE_CoOpen ���ص��˳̹�����
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
* ����һ���˳̶���,�����ش����˳̵�id. �����õ��˳�״̬��PUMP_COSTATUS_READY
* comng    : PUMP_CORE_CoOpen ���ص��˳̹�����
* func        : �˳����е�����
* ud        : �û����������, PUMP_CORE_CoPCallBack �� ud ��ʹ��
*            : ���ش����õ��˳̱�ʶid
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoCreate(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud) 
{
    struct __PUMP_COROUTINE * co = __PUMP_CORE_INNER_CoNew(comng, func, ud);
    // ��������ͨ���, �����Ҽ�
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

    // ��Ҫ���·���ռ�, ������Ϻ󷵻�
    comng->co = (struct __PUMP_COROUTINE **)realloc(comng->co, sizeof(struct __PUMP_COROUTINE *) * comng->cap * 2);
    assert(NULL != comng->co);
    memset(comng->co + comng->cap, 0, sizeof(struct __PUMP_COROUTINE *) * comng->cap);
    comng->cap <<= 1;
    comng->co[comng->nco] = co;
    return comng->nco++;
}

/*
* �õ���ǰ�˳����е�״̬
* comng    : �˳̹���������
* id        : PUMP_CORE_CoCreate �������˳̶���
*            : ����״̬������� PUMP_COSTATUS
*/
PUMP_CORE_API PUMP_COSTATUS PUMP_CALLBACK PUMP_CORE_CoGetStatus(pump_co_t comng, int id) 
{
    assert(comng && id >= 0 && id < comng->cap);
    return comng->co[id] ? comng->co[id]->status : PUMP_COSTATUS_DEAD;
}

/*
* �õ���ǰ�˳�ϵͳ�����е��˳�, ���� < 0��ʾû���˳�������
* comng    : �˳̹���������
*            : ���ص�ǰ���е��˳̱�ʶid,
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoGetRunning(pump_co_t comng) 
{
    return comng->running;
}
