//
// Created by yz on 19-5-26.
//
#include <cstdio>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <ctime>
//#ifdef linux
//#include <unistd.h>
//#include <sys/timerfd.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <sys/select.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#elif (defined _WIN32)
//#include <WinSock2.h>
//#endif // linux
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_register.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_event/handle_event/socket_event.h"
//#include "pump_event/handle_event/fd_event_listener2.h"
//#include "pump_event/handle_event/fd_event2.h"
#include "pump_event/handle_event/timer_event.h"
#include "pump_event/event_loop.h"
#include "pump_event/handle_event/handle_event_handler.h"

//using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Core::Net;
using namespace Pump::Event;
using namespace Pump::Event::HandleEvent;

namespace TestCase
{
#define NULLSTR ""
#define TEST_CASE(case_id_, cond_, pass_, block_) \
do { \
PUMP_CORE_LOG_ASSERT(cond_)<<"<test_case> [block] "<< case_id_ <<" "<< pass_; \
PUMP_CORE_INFO<<"<test_case> [pass] " << case_id_ << block_; \
} while(0);


// <test-0> [pass]
// 测试 CNewlyEventMgr 功能
// </test-0>
bool test_0()
{
    PUMP_CORE_INFO << "***** test-0 *****";
    Pump::Event::CNewlyEventMgr NEMgr;
    Pump::Event::CNewlyEvMgrGuiderForInner oNEMGuider4Collector(NEMgr);
    Pump::Event::CNewlyEvMgrGuiderForUser oNEMGuider4User(NEMgr);
    
    Pump::Event::HandleEvent::CHandleInsert *sockCreate1 = new Pump::Event::HandleEvent::CHandleInsert();
    Pump::Event::HandleEvent::CHandleInsert *sockCreate2 = new Pump::Event::HandleEvent::CHandleInsert();
    Pump::Event::HandleEvent::CHandleInsert *sockCreate3 = new Pump::Event::HandleEvent::CHandleInsert();
    Pump::Event::HandleEvent::CHandleInsert *sockCreate4 = new Pump::Event::HandleEvent::CHandleInsert();
    
    oNEMGuider4User.insert(sockCreate1);
    oNEMGuider4User.insert(sockCreate2);
    oNEMGuider4User.insert(sockCreate3);
    oNEMGuider4User.insert(sockCreate4);
    
    size_t iLen;
    iLen = oNEMGuider4User.get_offscreen_locked().get()->size();
    oNEMGuider4User.unlock_offscreen();
    
    TEST_CASE("test-0-0", iLen == 4, "\texpect 4 actual" << iLen, NULLSTR);
    
    oNEMGuider4Collector.swap();
    
    iLen = oNEMGuider4User.get_offscreen_locked().get()->size();
    oNEMGuider4User.unlock_offscreen();
    
    TEST_CASE("test-0-1", iLen == 0, "\texpect 0 actual" << iLen, NULLSTR);
    
    iLen = oNEMGuider4Collector.get_work_locked().get()->size();
    oNEMGuider4Collector.unlock_work();
    
    TEST_CASE("test-0-2", iLen == 4, "\texpect 4 actual" << iLen, NULLSTR);
    
    return true;
}

// <test-1>
// 测试 CEventRegister 功能
// </test-1>
bool test_1()
{
    PUMP_CORE_INFO << "***** test-1 *****";
    Pump::Event::CEventEngine eventEngine;
    Pump::Event::CEventRegister eventRegister(&eventEngine);
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    Pump::Event::CEventListener *pListener =
        Pump::Event::HandleEvent::CHandleEventListener::getInstance(
            eventEngine.get_event_collector(),
            &arg,
            sizeof(arg));
    
    Pump::Event::HandleEvent::CHandleInsert *sockCreate1 = new Pump::Event::HandleEvent::CHandleInsert();
    sockCreate1->m_fd = (pump_fd_t)0;
    Pump::Event::HandleEvent::CHandleInsert *sockCreate2 = new Pump::Event::HandleEvent::CHandleInsert();
    sockCreate2->m_fd = (pump_fd_t)1;
    Pump::Event::HandleEvent::CHandleInsert *sockCreate3 = new Pump::Event::HandleEvent::CHandleInsert();
    sockCreate3->m_fd = (pump_fd_t)2;
    Pump::Event::HandleEvent::CHandleInsert *sockCreate4 = new Pump::Event::HandleEvent::CHandleInsert();
    sockCreate4->m_fd = (pump_fd_t)3;
    int ret = 0;
    ret += eventEngine.get_event_collector().insert_newly_event(sockCreate1);
    ret += eventEngine.get_event_collector().insert_newly_event(sockCreate2);
    ret += eventEngine.get_event_collector().insert_newly_event(sockCreate3);
    ret += eventEngine.get_event_collector().insert_newly_event(sockCreate4);
    TEST_CASE("test-1-0", ret == 4, "\texpect 4 actual " << ret, NULLSTR);
    return true;
}

// <test-2>
// 测试 CEventLoop 线程控制功能
// </test-2>
bool test_2()
{
    Pump::Event::CEventLoop * loop1 = new Pump::Event::CEventLoop();
    TEST_CASE("test-2", loop1->Start() == 0, NULLSTR, NULLSTR);
    PUMP_CORE_INFO<<"loop1->start()";
#ifdef __linux__
    sleep(3);
#elif (defined _WIN32)
    ::Sleep(3000);
#endif // __linux__
    loop1->Pause();
    PUMP_CORE_INFO<<"loop1->pause()";
#ifdef __linux__
    sleep(3);
#elif (defined _WIN32)
    ::Sleep(3000);
#endif // __linux__
    loop1->Resume();
    PUMP_CORE_INFO<<"loop1->resume()";
#ifdef __linux__
    sleep(3);
#elif (defined _WIN32)
    ::Sleep(3000);
#endif // __linux__
    loop1->Stop();
    PUMP_CORE_INFO<<"loop1->stop()";
    return true;
}

// <test-3>
// 测试 CEventLoop 事件增加功能
// </test-3>
bool test_3()
{
    Pump::Event::CEventLoop * loop1 = new Pump::Event::CEventLoop();
    TEST_CASE("test-3-0", loop1->Start() == 0, NULLSTR, NULLSTR);
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    arg.emBackendType_ = ::Pump::Core::Net::PUMP_SELECT;
    Pump::Event::CEventListener * pFdListener = Pump::Event::HandleEvent::CHandleEventListener::getInstance(loop1->GetEventCollector(), &arg, sizeof(arg));
    loop1->InsertListener((pump_listener_handle)pFdListener);

#ifdef __linux__
    //<create_socket>
    Pump::Event::pump_fd_t fd;
    const char * szIp = "127.0.0.1";
    const int iPort = 10021;
    sockaddr_in servaddr;
    fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        PUMP_CORE_INFO << stderr << "create socket fail, erron: %d ,reason: %s\n" <<
            errno << strerror(errno);
        return false;
    }
    /*一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用*/
    int reuse = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        return false;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, szIp, &servaddr.sin_addr);
    servaddr.sin_port = ::htons(iPort);

    if (::bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        PUMP_CORE_INFO << "bind() error: " << errno;
        return -1;
    }
    //</create_socket>

    // 创建socket一次性事件
    Pump::Event::fd_op::CSockCreate * pEvSoCreate = new Pump::Event::fd_op::CSockCreate();
    pEvSoCreate->m_fd = fd;
    loop1->insert_event((pump_ev_handle)pEvSoCreate);
    delete pEvSoCreate;

    // 接收Accept socket持续性事件
    Pump::Event::fd_op::CSockAccept * pEvSoAccept = new Pump::Event::fd_op::CSockAccept();
    pEvSoAccept->m_fd = fd;
    Pump::Event::CEvent::EvData pData;
    pData.pData_ = (void*)loop1->get_tid();
    pData.bDel_ = false;
    pEvSoAccept->set_data(Pump::Event::EVSTATE_DESTROYED, pData);
    loop1->insert_event((pump_ev_handle)pEvSoAccept);

    sleep(60);

    Pump::Event::fd_op::CFdClose * pEvSoClose = new Pump::Event::fd_op::CFdClose();
    pEvSoClose->m_fd = fd;
    loop1->insert_event((pump_ev_handle)pEvSoClose);
    delete pEvSoAccept;
    delete pEvSoClose;
#else
    CSock *pSock = CSock::CreateSock(PUMP_AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!pSock)
    {
        return false;
    }
    //CAddr objAddr(PUMP_AF_INET, "127.0.0.1", 10002);
    //pSock->ListenExt(objAddr, 5);
    CHandleInsert * pEvHInsert = new CHandleInsert(pSock);
    loop1->InsertEvent((pump_ev_handle)pEvHInsert);
    CTCPSockAccept * pEvSoAccept = new CTCPSockAccept(pSock);
    CEvent::EvData pData;
    pData.pData_ = (void*)loop1->GetTID();
    pData.bDel_ = false;
    pEvSoAccept->set_data(Pump::Event::EVSTATE_DESTROYED, pData);
    pEvSoAccept->m_struArg.iAf_ = PUMP_AF_INET;
    strncpy(pEvSoAccept->m_struArg.szIp_, "127.0.0.1", strlen("127.0.0.1"));
    pEvSoAccept->m_struArg.sPort = 10002;
    pEvSoAccept->m_struArg.iBackLog_ = 5;
    loop1->InsertEvent((pump_ev_handle)pEvSoAccept);

    PUMP_CORE_Sleep(100000);
    pSock->Close();
    CHandleRemove * pEvSoClose = new CHandleRemove();
    pEvSoClose->m_pHandle = pSock;
    loop1->InsertEvent((pump_ev_handle)pEvSoClose);
    delete pEvSoAccept;
    delete pEvSoClose;
    delete loop1;
#endif // __linux__
    
    return true;
}
#ifdef __linux__
void timer_thread(union sigval v)
{
    printf("[-----------]timer_thread pump_boost! %d\n", v.sival_int);
}
#endif // __linux__

// <test-4>
// 测试 CEventLoop 事件增加功能
// </test-4>
bool test_4()
{
    Pump::Event::CEventLoop * loop1 = new Pump::Event::CEventLoop();
    TEST_CASE("test-4-0", loop1->Start() == 0, NULLSTR, NULLSTR);
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    arg.emBackendType_ = ::Pump::Core::Net::PUMP_SELECT;
    Pump::Event::CEventListener * pFdListener = Pump::Event::HandleEvent::CHandleEventListener::getInstance(loop1->GetEventCollector(), &arg, sizeof(arg));
    loop1->InsertListener((pump_listener_handle)pFdListener);

#ifdef __linux__
    //<create_socket>
    Pump::Event::pump_fd_t fd = ::timerfd_create(CLOCK_REALTIME, 0);
    struct itimerspec tspec_new, tspec_old;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    tspec_new.it_value.tv_sec = now.tv_sec + 3;
    tspec_new.it_value.tv_nsec = now.tv_nsec;
    tspec_new.it_interval.tv_sec = 10;
    
    ::timerfd_settime(fd, TFD_TIMER_ABSTIME, &tspec_new, &tspec_old);
    //</create_socket>
    
    // 创建socket一次性事件
    Pump::Event::fd_op::CTimerCreate * pEvTimerCreate = new Pump::Event::fd_op::CTimerCreate(Pump::Event::CEvent::CONTINUOUS);
    pEvTimerCreate->m_fd = fd;
    loop1->insert_event((pump_ev_handle)pEvTimerCreate);
    
    sleep(-1);
    delete pEvTimerCreate;
#endif /// __linux__
    return true;
}
#ifdef __linux__
void test_timer_t()
{
    timer_t fd;
    
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));	//清零初始化
    
    evp.sigev_value.sival_int = 111;		//也是标识定时器的，这和timerid有什么区别？回调函数可以获得
    evp.sigev_notify = SIGEV_THREAD;		//线程通知的方式，派驻新线程
    evp.sigev_notify_function = timer_thread;	//线程函数地址
    timer_create(CLOCK_REALTIME, &evp, &fd);
    //</create_socket>
    itimerspec it;
    it.it_interval.tv_sec = 1;  // 后续按照该时间间隔
    it.it_interval.tv_nsec = 0;
    it.it_value.tv_sec = 3;     // 最初开始时间间隔
    it.it_value.tv_nsec = 0;
    
    if (timer_settime(fd, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        return;
    }
}
#endif /// __linux__

class CMyTCPRecvHandler
    : public ::Pump::Event::HandleEvent::CTCPSockRecvHandler
{
public:
    CMyTCPRecvHandler() {}
    virtual ~CMyTCPRecvHandler() {}
    virtual pump_int32_t Open(pump_pvoid_t pParam)
    {
        if (!pParam)
        {
            return PUMP_ERROR;
        }
        CTCPSockRecvHandlerArgs * pArgs = static_cast<CTCPSockRecvHandlerArgs*>(pParam);
        CTCPSockRecv * pEvTcpRecv = new CTCPSockRecv();
        pEvTcpRecv->Bind(CTCPSockRecv::OnRecvCbType(&::Pump::Event::HandleEvent::CTCPSockRecvHandler::OnRecv, this));
        pEvTcpRecv->m_pHandle = pArgs->m_pSock;
        pEvTcpRecv->m_fd = pArgs->m_pSock->GetHandle();
        pArgs->m_handleCap->insert(pEvTcpRecv);
        return PUMP_OK;
    }
    virtual pump_int32_t Close(pump_pvoid_t pParam)
    {
        return PUMP_OK;
    }
    virtual pump_int32_t OnRecv(void *)
    {
        printf("[------YZ------] CMyTCPRecv::OnRecv()\n");
        return PUMP_OK;
    }
};

class CMyTCPAcceptHandler
    : public ::Pump::Event::HandleEvent::CTCPSockAcceptHandler
{
public:
    CMyTCPAcceptHandler() {}
    virtual ~CMyTCPAcceptHandler() {}
    virtual pump_int32_t OnAccept(::Pump::Core::Net::CSock * pSock, CHandleCap * handleCap, pump_pvoid_t pData)
    {
        PUMP_CORE_INFO << "CMyTCPAccept::OnAccept";
        // 允许读事件,投递读事件
        CMyTCPRecvHandler * pRecvHandler = new (std::nothrow)CMyTCPRecvHandler();
        if (!pRecvHandler)
        {
            return  PUMP_ERROR;
        }
        CMyTCPRecvHandler::CTCPSockRecvHandlerArgs objArgs;
        objArgs.m_pSock = pSock;
        objArgs.m_handleCap = handleCap;
        objArgs.m_pData = NULL;
        pRecvHandler->Open(&objArgs);
        m_vecRecvHandler.push_back(pRecvHandler);
        return PUMP_OK;
    }
private:
    std::vector<CMyTCPRecvHandler*> m_vecRecvHandler;
};

bool test_5()
{
    Pump::Event::CEventLoop * loop1 = new Pump::Event::CEventLoop();
    TEST_CASE("test-3-0", loop1->Start() == 0, NULLSTR, NULLSTR);
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    arg.emBackendType_ = ::Pump::Core::Net::PUMP_SELECT;
    Pump::Event::CEventListener * pFdListener = Pump::Event::HandleEvent::CHandleEventListener::getInstance(loop1->GetEventCollector(), &arg, sizeof(arg));
    loop1->InsertListener((pump_listener_handle)pFdListener);
    ::Pump::Event::HandleEvent::CTCPSockAcceptHandler objAcceptHandler;
    objAcceptHandler.SetEventLoop(loop1);
    objAcceptHandler.Open(NULL);
    getchar();
    delete loop1;
    return true;
}

bool test_6()
{
    Pump::Event::CEventLoop * loop1 = new Pump::Event::CEventLoop();
    TEST_CASE("test-3-0", loop1->Start() == 0, NULLSTR, NULLSTR);
    Pump::Event::HandleEvent::CHandleEventListener::HandleEventListenerArg arg;
    arg.emBackendType_ = ::Pump::Core::Net::PUMP_SELECT;
    Pump::Event::CEventListener * pFdListener = Pump::Event::HandleEvent::CHandleEventListener::getInstance(loop1->GetEventCollector(), &arg, sizeof(arg));
    loop1->InsertListener((pump_listener_handle)pFdListener);
    CMyTCPAcceptHandler objAcceptHandler;
    objAcceptHandler.SetEventLoop(loop1);
    objAcceptHandler.Open(NULL);
    getchar();
    delete loop1;
    return true;
}

// <test-x>
bool test_x()
{
//  CEventCollector evCollector;
//  fd_event_listener::fd_event_listener_arg arg;
//  Pump::CEventListener *pListener =
//    fd_op::fd_event_listener::getInstance(evCollector,
//                                    &arg,
//                                    sizeof(arg));
//
//  fd_event_listener2::fd_event_listener2_arg arg2;
//  Pump::CEventListener *pListener2 =
//    fd_op::fd_event_listener2::getInstance(evCollector,
//                                     &arg2,
//                                     sizeof(arg2));
//
//  PUMP_CORE_LOG_ASSERT(pListener && pListener2) << "x-1 block";
//
//  // test 1-1
//  PUMP_CORE_LOG_ASSERT(!(evCollector.get_listened_event_mgr().get_map_listened_event().size() == 2)) << "1-2 pass";
//
//  fd_op::CSockAccept *hEvnt = new fd_op::CSockAccept();
//  hEvnt->m_fd = 1;
//  PUMP_CORE_LOG_ASSERT(evCollector.get_listened_event_mgr().insert_event((pump_handle_t) hEvnt)) << "1-3 block";
//
//  fd_op::CSockAccept2 *hEvnt2 = new fd_op::CSockAccept2();
//  PUMP_CORE_LOG_ASSERT(evCollector.get_listened_event_mgr().insert_event((pump_handle_t) hEvnt2)) << "1-4 block";
//
//  pump_event *aaa = ((lp_fd_event_hash_container) evCollector.get_listened_event_mgr().get_map_listened_event().find(
//    (pump_handle_t) (((fd_event_listener *) pListener)->get_fd_hash_container()))->second->_M_p_ev_conatiner)->get_arr_fd()[hEvnt->m_fd].m_mapEv.find(
//    FDEV_TYPE_SOCK_ACCEPT)->second.get();
//  PUMP_CORE_LOG_ASSERT((evCollector.get_listened_event_mgr().get_map_listened_event().find(
//    (pump_handle_t)(((fd_event_listener*)pListener)->get_fd_hash_container()))->second->_M_p_ev_conatiner)->get_arr_fd()[hEvnt->m_fd].getMapEvent().find(
//    FDEV_TYPE_ACCEPT)
//             != ((CFdEventHashContainer *) evCollector.get_listened_event_mgr().get_map_listened_event().find(
//    (pump_handle_t) pListener)->second->_M_p_ev_conatiner)->get_arr_fd()[hEvnt->m_fd].getMapEvent().end())
//  << "1-3 block";
//
//  PUMP_CORE_LOG_ASSERT(((CFdEventHashContainer *) evCollector.get_listened_event_mgr().get_map_listened_event().find(
//    (pump_handle_t) pListener2)->second->_M_p_ev_conatiner)->get_arr_fd()[hEvnt2->m_fd].getMapEvent().find(
//    FDEV_TYPE_ACCEPT)
//             != ((CFdEventHashContainer *) evCollector.get_listened_event_mgr().get_map_listened_event().find(
//    (pump_handle_t) pListener2)->second->_M_p_ev_conatiner)->get_arr_fd()[hEvnt2->m_fd].getMapEvent().end())
//  << "1-4 block";
    
    return true;
}

}

using namespace TestCase;

int main(int argc, char** argv)
{
    PUMP_CORE_InitNetwork();
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    strcpy(struLogCong.szFilePath, argv[0]);
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    //PUMP_CORE_InitLogger(&struLogCong);
//  TEST_CASE("test-0", TestCase::test_0(), NULLSTR, NULLSTR);
//  TEST_CASE("test-1", TestCase::test_1(), NULLSTR, NULLSTR);
//  TEST_CASE("test-2", TestCase::test_2(), NULLSTR, NULLSTR);
    //TEST_CASE("test-3", TestCase::test_3(), NULLSTR, NULLSTR);
    //TEST_CASE("test-4", TestCase::test_4(), NULLSTR, NULLSTR);
    //TEST_CASE("test-5", TestCase::test_5(), NULLSTR, NULLSTR);
    TEST_CASE("test-6", TestCase::test_6(), NULLSTR, NULLSTR);
    return getchar();
}
