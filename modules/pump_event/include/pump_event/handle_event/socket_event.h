//
// Created by yz on 19-6-8.
//

#ifndef PUMP_EVENT_SOCKETEVENT_H
#define PUMP_EVENT_SOCKETEVENT_H
#include "pump_macro/pump_pre.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_memory/smart_buffer/smart_buffer.h"
#include "pump_function/pump_function.hpp"
#include "pump_event/pump_event_config.h"
#include "pump_event/handle_event/handle_event.h"
#include "pump_event/handle_event/handle_cap.h"
#include "pump_event/handle_event/handle_event_handler.h"

//using namespace Pump::Memory;

namespace Pump
{
namespace Event
{

class CEventLoop;

namespace HandleEvent
{

class CHandleCap;

/**
 * @brief [持续事件] sock accept事件，监听事件
 */
class PUMP_EVENT_CLASS CTCPSockAccept
    : public CHandleEvent
{
public:
    struct SockAcceptArg
    {
        pump_int32_t iAf_;
        pump_char_t szIp_[PUMP_MAX_IP_ADDRLEN];
        pump_uint16_t sPort;
        pump_int32_t iBackLog_;
    };
    typedef ::Pump::Function::CFunction<
        PUMP_MFN_TYPE(CTCPSockAcceptHandler, 
        pump_int32_t, 
        (::Pump::Core::Net::CSock *, CHandleCap*, pump_pvoid_t))
    > OnAcceptCbType;
public:
    CTCPSockAccept();
    CTCPSockAccept(OnAcceptCbType & cb);

    CTCPSockAccept(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior = EVPRIOR_DEFAULT);

    virtual ~CTCPSockAccept();
    void Bind(OnAcceptCbType & cb);
    /**
     * @fn _on_listend
     * @param refEvEngine 事件驱动引擎
     * @param objData 参数
     * @return 状态码
     * @brief 启动套接字监听, 向 Backend 添加IO事件
     */
    virtual int _on_listend(CEventEngine &refEvEngine, EvData objData);

    /**
     * @fn _on_actived
     * @param refEvEngine
     * @param objData
     * @return 状态码
     * @brief 有连接请求, 调用accept(), 调用用户回调
     */
    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData);

    /**
   * @fn _on_destroyed
   * @param refEvEngine
   * @param objData
   * @return 状态码
   * @brief 销毁套接字
   */
    virtual int _on_destroyed(CEventEngine &refEvEngine, EvData objData);

    /**accept参数*/
    SockAcceptArg m_struArg;
    /**用户回调*/
    OnAcceptCbType m_cbAccept;
};

/**
 * @struct IoEvent []
 * @brief IO 事件,封装socket相关的属性及操作
 */
class PUMP_EVENT_CLASS CTCPSockConnect
    : public CHandleEvent
{
public:

    CTCPSockConnect();

    CTCPSockConnect(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior);

    virtual ~CTCPSockConnect();

    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData);
};

/**
 * @struct IoEvent []
 * @brief IO 事件,封装socket相关的属性及操作
 */
class PUMP_EVENT_CLASS CTCPSockRecv
    : public CHandleEvent
{
public:
    typedef ::Pump::Function::CFunction<
        PUMP_MFN_TYPE(CTCPSockRecvHandler,
        pump_int32_t,
        (pump_pvoid_t))
    > OnRecvCbType;
public:
    /**
     * @var PtrIoBuffer m_pIobufRecv
     * @brief 输入缓冲区
     * 注: 仅io套接字有效
     */
    ::Pump::Memory::IoBuffer m_objIoBufRecv;

    CTCPSockRecv();

    CTCPSockRecv(OnRecvCbType & cb);

    CTCPSockRecv(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior);

    virtual ~CTCPSockRecv();

    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData);
    void Bind(OnRecvCbType & cb);
protected:
    pump_int32_t __OnRemoteClose(CHandleCap * pFDescriptor, CEventEngine &refEvEngine);
    pump_int32_t __OnRecv();
protected:
    OnRecvCbType m_cbRecv;
};

/**
 * @struct IoEvent []
 * @brief IO 事件,封装socket相关的属性及操作
 * TODO 缺少用户回调
 */
class PUMP_EVENT_CLASS CTCPSockSend
    : public CHandleEvent
{
    /**
     * @var PtrIoBuffer m_pIobufSend
     * @brief 输出缓冲区
     * 注: 仅io套接字有效
     */
    ::Pump::Memory::IoBuffer m_objIoBufSend;

    CTCPSockSend();

    CTCPSockSend(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior);

    virtual ~CTCPSockSend();

    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData);
};

}
}
}

#endif // PUMP_EVENT_SOCKETEVENT_H
