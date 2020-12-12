//
// Created by yz on 19-6-7.
//

#ifndef PUMP_EVENT_HANDLE_DEF_H
#define PUMP_EVENT_HANDLE_DEF_H

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

enum FdEventType
{
    FDEV_TYPE_SOCK_CREART,    //! 创建Socket
    FDEV_TYPE_SOCK_ACCEPT,    //! 接受TCP连接
    FDEV_TYPE_SOCK_CONNECT,   //! 建立TCP连接
    FDEV_TYPE_SOCK_RECV,      //! 从TCP socket接收
    FDEV_TYPE_SOCK_SEND,      //! 向TCP socket发送
    FDEV_TYPE_SOCK_SHUTDOWN,  //! TCP 连接半关闭
    FDEV_TYPE_FILE_OPEN,      //! 打开文件描述符(本地)
    FDEV_TYPE_FILE_READ,      //! 从fd读
    FDEV_TYPE_FILE_WRITE,     //! 向fd写
    FDEV_TYPE_TIMER_CREART,   //! 创建定时器事件
    FDEV_TYPE_TIMER_START,    //! 启动定时器事件
    FDEV_TYPE_CLOSE           //! 关闭fd
};

/**
 * @enum FdState
 * @brief 文件描述符的生命周期状态
 *
 * - FD_STATE_LISTENED
 * - FD_STATE_CONNECTING
 * - FD_STATE_CONNECTED
 * - FD_STATE_SHUTDOWNED
 * - FD_STATE_CLOSED
 */
enum FdState
{
    FD_STATE_DEFAULT = 0,      //! fd 无状态
    FD_STATE_SOCK_INITED,      //! fd sock 已初始化
    FD_STATE_SOCK_LISTENED,    //! fd sock 正在监听(接受连接请求<服务端>)
    FD_STATE_SOCK_CONNECTING,  //! fd sock 正在连接(异步<客户端>)
    FD_STATE_SOCK_CONNECTED,   //! fd sock 已连接(异步<客户端/服务端>)
    FD_STATE_SOCK_SHUTDOWNED,  //! fd sock 半闭态(异步<客户端/服务端>)
    FD_STATE_TIMER_OPEN,       //! fd timer 打开状态
    FD_STATE_CLOSED            //! fd 关闭态(异步<客户端/服务端>)
};

}
}
}

#endif //PUMP_EVENT_HANDLE_DEF_H
