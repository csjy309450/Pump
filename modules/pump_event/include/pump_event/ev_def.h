/**
* @file pumpdef.h
* @brief 定义 libpump 库的全局类型, 将来可能拆分成多个文件
*
* @author YangZheng 263693992\qq.com
* @version 1.0
* @date 2018.07.29
*/

#ifndef PUMP_EVENT_DEF_H
#define PUMP_EVENT_DEF_H

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "pump_macro/pump_pre.h"
#include <cstring>
#include "pump_core/os_wrapper/pump_core_os_types.h"

#if (defined PUMP_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
typedef pump_handle_t pump_ev_handle;
typedef pump_handle_t pump_listener_handle;
typedef pump_handle_t pump_container_handle;
#elif (defined PUMP_OS_POSIX)
typedef pump_handle_t pump_ev_handle;
typedef pump_handle_t pump_listener_handle;
typedef pump_handle_t pump_container_handle;
#endif // (defined PUMP_OS_WINDOWS)

namespace Pump
{
namespace Event
{

/** 事件优先级的数值类型, 与 enum EventPriority 对应 */
typedef unsigned short ev_prior_t;

#define PUMP_EVENT_IS_HANDLE_AVAIL(_handle) ((pump_handle_t)_handle==0 || ((pump_handle_t)(void*)-1)==(pump_handle_t)_handle)

/**
* @enum EventPriority
* @brief Event 优先级
*
* 定义了事件对象 Event 优先级, 从上往下优先级由高到低
*/
enum EventPriority
{
    EVPRIOR_DEFAULT = 0, //! 默认优先级, 即最低
    EVPRIOR_LEVEL1,      //! 次高优先级
    EVPRIOR_HIGHEST      //! 最高优先级
};

/**
* @enum EventType
* @brief Event 生命周期状态
*
* 定义了事件对象 Event 生命周期状态
* > EVSTATE_NEWED 新建态,事件加入到NewlyEventMgr时
* > EVSTATE_LISTENED 监听态,新建态条件事件注册到ListenedEventMgr
* > EVSTATE_ACTIVATED 激活态,立即事件注册到ActivatedEventMgr
* > EVSTATE_SUSPEND 挂起态,持续性事件可以暂时被挂起不执行
* > EVSTATE_SOLVED 事件已处理态
*
* 状态转换规则
* EVSTATE_ACTIVATED <- EVSTATE_NEWLY -> EVSTATE_LISTENED
* EVSTATE_LISTENED <-> EVSTATE_ACTIVE
* EVSTATE_ACTIVE -> EVSTATE_SOLVED -> EVSTATE_REGISTERED
*/
enum EventState
{
    EVSTATE_NEWED = 0,       //! 新建态,事件加入到NewlyEventMgr时
    EVSTATE_LISTENED,        //! 监听态,新建态条件事件注册到ListenedEventMgr
    EVSTATE_ACTIVATED,       //! 激活态,立即事件注册到ActivatedEventMgr
    EVSTATE_SUSPEND,         //! 挂起态,持续性事件可以暂时被挂起不执行
    EVSTATE_SOLVED,          //! 已处理态
    EVSTATE_DESTROYED        //! 销毁态
};

/**
* \enum EventType
* \brief Event 基本类型
*
* 定义了事件对象 Event 基本类型
* > EVTYPE_PRE 事前事件, 加入到 PWatcher 的 PreEventsList 中在每一轮 preWatch() 中被执行
* > EVTYPE_NORM 普通事件, 加入到 PWatcher 的 NormEventsList 中在每一轮 watch() 中检测并激活
* > EVTYPE_POST 事后事件, 加入到 PWatcher 的 PostEventsList 中在每一轮 postWatch() 中被执行
*/
enum EventType
{
    EVTYPE_PRE,   //! 前置事件
    EVTYPE_NORM,  //! 普通事件
    EVTYPE_POST,  //! 后置事件
};

/**
* \enum enum PumpState
* \brief Pump 生命周期状态
*
* 定义了事件循环对象 Pump 生命周期状态
* > PUMPSTATE_NEW 表示 Pump 对象刚刚构造, 需要初始化 WatcherList 后才能运行
* > PUMPSTATE_INIT 表示 Pump 对象完成初始化
* > PUMPSTATE_STOP 表示 Pump 对象未启动运行
* > PUMPSTATE_START 表示 Pump 对象正在运行
* > PUMPSTATE_PAUSE　表示 Pump 对象处于挂起(暂停)状态
*
* 状态转换规则
* PUMPSTATE_NEW　-> PUMPSTATE_INIT
* PUMPSTATE_INIT -> PUMPSTATE_STOP
* PUMPSTATE_INIT -> PUMPSTATE_START
* PUMPSTATE_INIT -> PUMPSTATE_PAUSE
* PUMPSTATE_STOP <-> PUMPSTATE_START <-> PUMPSTATE_PAUSE
*/
enum PumpState
{
    PUMPSTATE_NEW,    //! 新建态
    PUMPSTATE_INIT,   //! 初始化态
    PUMPSTATE_START,  //! 运行态
    PUMPSTATE_PAUSE,  //! 暂停态
    PUMPSTATE_WAITE,  //! 挂起态
    PUMPSTATE_STOP,   //! 终止态
};

enum PumpType
{
    PUMPTYPE_SYNC,  //! 同步执行Pump
    PUMPTYPE_ASYNC, //! 异步执行Pump
};

/**
* \enum TimerType
* \brief 定时器类型
*
* - TMTYPE_ONETIME 一次性定时器
* - TMTYPE_PERIODIC 周期性定时器
*/
enum TimerType
{
    TMTYPE_DEFAULT,      //! 默认类型
    TMTYPE_ONETIME,   //! 一次性定时器
    TMTYPE_PERIODIC   //! 周期性定时器
};

}
}

#endif //PUMP_EVENT_DEF_H
