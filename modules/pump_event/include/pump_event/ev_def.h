/**
* @file pumpdef.h
* @brief ���� libpump ���ȫ������, �������ܲ�ֳɶ���ļ�
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

/** �¼����ȼ�����ֵ����, �� enum EventPriority ��Ӧ */
typedef unsigned short ev_prior_t;

#define PUMP_EVENT_IS_HANDLE_AVAIL(_handle) ((pump_handle_t)_handle==0 || ((pump_handle_t)(void*)-1)==(pump_handle_t)_handle)

/**
* @enum EventPriority
* @brief Event ���ȼ�
*
* �������¼����� Event ���ȼ�, �����������ȼ��ɸߵ���
*/
enum EventPriority
{
    EVPRIOR_DEFAULT = 0, //! Ĭ�����ȼ�, �����
    EVPRIOR_LEVEL1,      //! �θ����ȼ�
    EVPRIOR_HIGHEST      //! ������ȼ�
};

/**
* @enum EventType
* @brief Event ��������״̬
*
* �������¼����� Event ��������״̬
* > EVSTATE_NEWED �½�̬,�¼����뵽NewlyEventMgrʱ
* > EVSTATE_LISTENED ����̬,�½�̬�����¼�ע�ᵽListenedEventMgr
* > EVSTATE_ACTIVATED ����̬,�����¼�ע�ᵽActivatedEventMgr
* > EVSTATE_SUSPEND ����̬,�������¼�������ʱ������ִ��
* > EVSTATE_SOLVED �¼��Ѵ���̬
*
* ״̬ת������
* EVSTATE_ACTIVATED <- EVSTATE_NEWLY -> EVSTATE_LISTENED
* EVSTATE_LISTENED <-> EVSTATE_ACTIVE
* EVSTATE_ACTIVE -> EVSTATE_SOLVED -> EVSTATE_REGISTERED
*/
enum EventState
{
    EVSTATE_NEWED = 0,       //! �½�̬,�¼����뵽NewlyEventMgrʱ
    EVSTATE_LISTENED,        //! ����̬,�½�̬�����¼�ע�ᵽListenedEventMgr
    EVSTATE_ACTIVATED,       //! ����̬,�����¼�ע�ᵽActivatedEventMgr
    EVSTATE_SUSPEND,         //! ����̬,�������¼�������ʱ������ִ��
    EVSTATE_SOLVED,          //! �Ѵ���̬
    EVSTATE_DESTROYED        //! ����̬
};

/**
* \enum EventType
* \brief Event ��������
*
* �������¼����� Event ��������
* > EVTYPE_PRE ��ǰ�¼�, ���뵽 PWatcher �� PreEventsList ����ÿһ�� preWatch() �б�ִ��
* > EVTYPE_NORM ��ͨ�¼�, ���뵽 PWatcher �� NormEventsList ����ÿһ�� watch() �м�Ⲣ����
* > EVTYPE_POST �º��¼�, ���뵽 PWatcher �� PostEventsList ����ÿһ�� postWatch() �б�ִ��
*/
enum EventType
{
    EVTYPE_PRE,   //! ǰ���¼�
    EVTYPE_NORM,  //! ��ͨ�¼�
    EVTYPE_POST,  //! �����¼�
};

/**
* \enum enum PumpState
* \brief Pump ��������״̬
*
* �������¼�ѭ������ Pump ��������״̬
* > PUMPSTATE_NEW ��ʾ Pump ����ոչ���, ��Ҫ��ʼ�� WatcherList ���������
* > PUMPSTATE_INIT ��ʾ Pump ������ɳ�ʼ��
* > PUMPSTATE_STOP ��ʾ Pump ����δ��������
* > PUMPSTATE_START ��ʾ Pump ������������
* > PUMPSTATE_PAUSE����ʾ Pump �����ڹ���(��ͣ)״̬
*
* ״̬ת������
* PUMPSTATE_NEW��-> PUMPSTATE_INIT
* PUMPSTATE_INIT -> PUMPSTATE_STOP
* PUMPSTATE_INIT -> PUMPSTATE_START
* PUMPSTATE_INIT -> PUMPSTATE_PAUSE
* PUMPSTATE_STOP <-> PUMPSTATE_START <-> PUMPSTATE_PAUSE
*/
enum PumpState
{
    PUMPSTATE_NEW,    //! �½�̬
    PUMPSTATE_INIT,   //! ��ʼ��̬
    PUMPSTATE_START,  //! ����̬
    PUMPSTATE_PAUSE,  //! ��̬ͣ
    PUMPSTATE_WAITE,  //! ����̬
    PUMPSTATE_STOP,   //! ��ֹ̬
};

enum PumpType
{
    PUMPTYPE_SYNC,  //! ͬ��ִ��Pump
    PUMPTYPE_ASYNC, //! �첽ִ��Pump
};

/**
* \enum TimerType
* \brief ��ʱ������
*
* - TMTYPE_ONETIME һ���Զ�ʱ��
* - TMTYPE_PERIODIC �����Զ�ʱ��
*/
enum TimerType
{
    TMTYPE_DEFAULT,      //! Ĭ������
    TMTYPE_ONETIME,   //! һ���Զ�ʱ��
    TMTYPE_PERIODIC   //! �����Զ�ʱ��
};

}
}

#endif //PUMP_EVENT_DEF_H
