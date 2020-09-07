/**
 * @file pump_core_coroutine.h
 * @brief Library Header.
 * @details Declaration of coroutine Implementation.
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

#ifndef PUMP_CORE_COROUTINE_H
#define PUMP_CORE_COROUTINE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"

typedef enum tagPUMP_COSTATUS {       // 纤程存在状态
    PUMP_COSTATUS_DEAD           = 0,        // 纤程死亡状态
    PUMP_COSTATUS_READY         = 1,        // 纤程已经就绪
    PUMP_COSTATUS_RUNNING   = 2,        // 纤程正在运行
    PUMP_COSTATUS_SUSPEND    = 3,        // 纤程暂停等待
} PUMP_COSTATUS;

typedef struct _PUMP_CO_CONTEX * pump_co_t;

/*
* 创建运行纤程的主体, 等同于纤程创建需要执行的函数体.
* schedule : PUMP_CORE_CoOpen 函数返回的结果
* ud       : 用户自定义数据
*/
typedef void(*PUMP_CORE_CoPCallBack)(pump_co_t comng, void * ud);

/*
* 开启纤程系统, 并创建主纤程
*            : 返回开启的纤程调度系统管理器
*/
PUMP_CORE_API pump_co_t PUMP_CALLBACK PUMP_CORE_CoOpen(void);

/*
* 关闭开启的纤程系统
* comng    : PUMP_CORE_CoOpen 返回的纤程管理器
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoClose(pump_co_t comng);

/*
* 创建一个纤程对象,并返回创建纤程的id. 创建好的纤程状态是PUMP_COSTATUS_READY
* comng    : PUMP_CORE_CoOpen 返回的纤程管理器
* func     : 纤程运行的主体
* ud       : 用户传入的数据, PUMP_CORE_CoPCallBack 中 ud 会使用
*          : 返回创建好的纤程标识id
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoCreate(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud);

/*
* 激活创建的纤程对象.
* comng    : 纤程管理器对象
* id       : PUMP_CORE_CoCreate 创建的纤程对象
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoResume(pump_co_t comng, int id);

/*
* 中断当前运行的的纤程, 并将CPU交给主纤程处理调度.
* comng    : 纤程管理器对象
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoYield(pump_co_t comng);

/*
* 得到当前纤程运行的状态
* comng    : 纤程管理器对象
* id       : PUMP_CORE_CoCreate 创建的纤程对象
*          : 返回状态具体参照 PUMP_COSTATUS
*/
PUMP_CORE_API PUMP_COSTATUS PUMP_CALLBACK PUMP_CORE_CoGetStatus(pump_co_t comng, int id);

/*
* 得到当前纤程系统中运行的纤程, 返回 < 0表示没有纤程在运行
* comng    : 纤程管理器对象
*          : 返回当前运行的纤程标识id,
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoGetRunning(pump_co_t comng);

#endif // PUMP_CORE_COROUTINE

