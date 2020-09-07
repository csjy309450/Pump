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

typedef enum tagPUMP_COSTATUS {       // �˳̴���״̬
    PUMP_COSTATUS_DEAD           = 0,        // �˳�����״̬
    PUMP_COSTATUS_READY         = 1,        // �˳��Ѿ�����
    PUMP_COSTATUS_RUNNING   = 2,        // �˳���������
    PUMP_COSTATUS_SUSPEND    = 3,        // �˳���ͣ�ȴ�
} PUMP_COSTATUS;

typedef struct _PUMP_CO_CONTEX * pump_co_t;

/*
* ���������˳̵�����, ��ͬ���˳̴�����Ҫִ�еĺ�����.
* schedule : PUMP_CORE_CoOpen �������صĽ��
* ud       : �û��Զ�������
*/
typedef void(*PUMP_CORE_CoPCallBack)(pump_co_t comng, void * ud);

/*
* �����˳�ϵͳ, ���������˳�
*            : ���ؿ������˳̵���ϵͳ������
*/
PUMP_CORE_API pump_co_t PUMP_CALLBACK PUMP_CORE_CoOpen(void);

/*
* �رտ������˳�ϵͳ
* comng    : PUMP_CORE_CoOpen ���ص��˳̹�����
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoClose(pump_co_t comng);

/*
* ����һ���˳̶���,�����ش����˳̵�id. �����õ��˳�״̬��PUMP_COSTATUS_READY
* comng    : PUMP_CORE_CoOpen ���ص��˳̹�����
* func     : �˳����е�����
* ud       : �û����������, PUMP_CORE_CoPCallBack �� ud ��ʹ��
*          : ���ش����õ��˳̱�ʶid
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoCreate(pump_co_t comng, PUMP_CORE_CoPCallBack func, void * ud);

/*
* ��������˳̶���.
* comng    : �˳̹���������
* id       : PUMP_CORE_CoCreate �������˳̶���
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoResume(pump_co_t comng, int id);

/*
* �жϵ�ǰ���еĵ��˳�, ����CPU�������˳̴������.
* comng    : �˳̹���������
*/
PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_CoYield(pump_co_t comng);

/*
* �õ���ǰ�˳����е�״̬
* comng    : �˳̹���������
* id       : PUMP_CORE_CoCreate �������˳̶���
*          : ����״̬������� PUMP_COSTATUS
*/
PUMP_CORE_API PUMP_COSTATUS PUMP_CALLBACK PUMP_CORE_CoGetStatus(pump_co_t comng, int id);

/*
* �õ���ǰ�˳�ϵͳ�����е��˳�, ���� < 0��ʾû���˳�������
* comng    : �˳̹���������
*          : ���ص�ǰ���е��˳̱�ʶid,
*/
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CoGetRunning(pump_co_t comng);

#endif // PUMP_CORE_COROUTINE

