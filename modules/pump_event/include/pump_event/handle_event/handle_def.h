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
    FDEV_TYPE_SOCK_CREART,    //! ����Socket
    FDEV_TYPE_SOCK_ACCEPT,    //! ����TCP����
    FDEV_TYPE_SOCK_CONNECT,   //! ����TCP����
    FDEV_TYPE_SOCK_RECV,      //! ��TCP socket����
    FDEV_TYPE_SOCK_SEND,      //! ��TCP socket����
    FDEV_TYPE_SOCK_SHUTDOWN,  //! TCP ���Ӱ�ر�
    FDEV_TYPE_FILE_OPEN,      //! ���ļ�������(����)
    FDEV_TYPE_FILE_READ,      //! ��fd��
    FDEV_TYPE_FILE_WRITE,     //! ��fdд
    FDEV_TYPE_TIMER_CREART,   //! ������ʱ���¼�
    FDEV_TYPE_TIMER_START,    //! ������ʱ���¼�
    FDEV_TYPE_CLOSE           //! �ر�fd
};

/**
 * @enum FdState
 * @brief �ļ�����������������״̬
 *
 * - FD_STATE_LISTENED
 * - FD_STATE_CONNECTING
 * - FD_STATE_CONNECTED
 * - FD_STATE_SHUTDOWNED
 * - FD_STATE_CLOSED
 */
enum FdState
{
    FD_STATE_DEFAULT = 0,      //! fd ��״̬
    FD_STATE_SOCK_INITED,      //! fd sock �ѳ�ʼ��
    FD_STATE_SOCK_LISTENED,    //! fd sock ���ڼ���(������������<�����>)
    FD_STATE_SOCK_CONNECTING,  //! fd sock ��������(�첽<�ͻ���>)
    FD_STATE_SOCK_CONNECTED,   //! fd sock ������(�첽<�ͻ���/�����>)
    FD_STATE_SOCK_SHUTDOWNED,  //! fd sock ���̬(�첽<�ͻ���/�����>)
    FD_STATE_TIMER_OPEN,       //! fd timer ��״̬
    FD_STATE_CLOSED            //! fd �ر�̬(�첽<�ͻ���/�����>)
};

}
}
}

#endif //PUMP_EVENT_HANDLE_DEF_H
