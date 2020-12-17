//
// Created by yz on 18-10-20.
//

#include <vector>
#include <list>
#include <deque>
#include <stdio.h>
#include <stdlib.h>
#if (defined WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // WIN32

//#include <boost/thread.hpp>
//#include <boost/thread/mutex.hpp>
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_thread.h"
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_app.h"
#include "pump_memory/smart_ptr/block.hpp"
#include "pump_memory/smart_ptr/policy.hpp"
#include "pump_memory/smart_ptr/voidsptr.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"
#include "pump_memory/smart_ptr/sharedptr.hpp"
#include "pump_memory/smart_ptr/weakptr.hpp"
#include "pump_memory/trie.hpp"
#include "pump_memory/smart_buffer/smart_buffer.h"
#include "pump_test/pump_test.h"

using namespace std;
using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Memory;
using namespace Pump::Core::Thread;
using namespace Pump::Test;

PTEST_C_SCENE_DEF(PumpMemoryUnitTestScene000, 
public:
    virtual int Init()
    {
        PTEST_ASSERT((CApplication::IsInit() == PUMP_OK), "PUMP_CORE_Init failed 3");
        PUMP_CORE_LOG_CONF struLogCong;
        memset(&struLogCong, 0, sizeof(struLogCong));
        struLogCong.bPrintConsole = PUMP_TRUE;
        struLogCong.bWriteFile = PUMP_TRUE;
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        strcpy(struLogCong.szFilePath, "yz_log_text");
        struLogCong.emLogLevel = PUMP_LOG_INFO;
        pump_handle_t hLog = PUMP_CORE_LoggerCreate();
        PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
        PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
        PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
        return 0;
    }
    virtual int Cleanup()
    {
        return 0;
    }
private:
    CApplication m_app;
)

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase001, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase001 test_Block");
    Heap<> heap0;
    // test 1
    PTEST_ASSERT((heap0.empty() == true), "[ ERROR ] 1-1");
    heap0.allocate(0);
    PTEST_ASSERT((heap0.block_segsz_ == 0 && heap0.empty()), "[ ERROR ] 1-2");
    heap0.allocate(10);
    PTEST_ASSERT((heap0.block_segsz_ == 10 && !heap0.empty()), "[ ERROR ] 1-3");
    heap0.deallocate();
    PTEST_ASSERT((heap0.block_segsz_ == 0 && heap0.empty()), "[ ERROR ] 1-4");
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase002, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase002 test_Block");
    // test 2
    std::allocator<char> a;
    HeapGuider<> bmgr0;
    HeapGuider<> bmgr1(sizeof(int), a);
    HeapGuider<> bmgr2(sizeof(int));
    PTEST_ASSERT((bmgr0.empty()), "[ ERROR ] 2-1");
    bmgr1.ref<int>() = 10;
    bmgr2.ref<int>() = 100;
    PTEST_ASSERT((!bmgr1.empty() && bmgr1.ref<int>() == 10), "[ ERROR ] 2-2");
    bmgr0 = bmgr1;
    PTEST_ASSERT((bmgr0.relative(bmgr1) == RELATIVE_EQUAL), "[ ERROR ] 2-3");
    HeapGuider<> bmgr1_2(bmgr1);
    PTEST_ASSERT((bmgr1_2.relative(bmgr1) == RELATIVE_EQUAL), "[ ERROR ] 2-4");
    bmgr1.swap(bmgr2);
    PTEST_ASSERT((bmgr1.ref<int>() == 100 && bmgr2.ref<int>() == 10), "[ ERROR ] 2-5");

    return 0;
}

class test_del {
public:
    void del() {
        delete(this);
    }
    ~test_del() {
        PUMP_CORE_INFO("~test_del");
    }
};

class TDel : public _Del {
public:
    virtual void pre_del(const Block *p) {
        PUMP_CORE_INFO("call TDel::pre_del");
    }
    void post_del(void * data) {
        PUMP_CORE_INFO("call TDel::post_del");
    }
};

PTEST_G_FN_DEF(TEST_VoidSPtr_1)
{
    VoidSPtr sp1_1;
    VoidSPtr sp1_2;
    // test 1
    PTEST_ASSERT((sp1_1 == PUMP_NULLPTR)
        && (sp1_1.capacity() == 0)
        && (sp1_2 == PUMP_NULLPTR)
        && (sp1_2 != sp1_1), "[ ERROR ] 1-1");
    VoidSPtr sp2(100000);
    PTEST_ASSERT((sp2.capacity() == 100000), "[ ERROR ] 1-2");

    // test 2
    TDel d0;
    VoidSPtr sp3(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp4(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp5(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp6(100000, PUMP_NULLPTR, d0);
    PTEST_ASSERT((sp3.capacity() == 100000), "[ ERROR ] 1-3");
    sp3.reset();
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase003, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase003 test_VoidSPtr_1");
    PTEST_G_FN_CALL(TEST_VoidSPtr_1);
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase004, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase004 test_voidwptr");
    VoidSPtr sp_0(10);
    VoidSPtr sp_1;
    VoidWPtr wp_0;

    PTEST_ASSERT((wp_0.expired()), "[ ERROR ] 1-1");
    wp_0 = sp_0;
    PTEST_ASSERT(!(wp_0.expired()), "[ ERROR ] 1-2");
    sp_1 = wp_0.lock_raw();
    PTEST_ASSERT((sp_1 == sp_0), "[ ERROR ] 1-3");
    VoidWPtr wp_1(sp_1);
    PTEST_ASSERT((wp_1 == wp_0), "[ ERROR ] 1-4");
    return true;
}

/////////////////////////////////////
//    </>VoidSPtr Test Case</>
/////////////////////////////////////
namespace nm_test_VoidSPtr {

static list<VoidSPtr> g_container[5];
static CMutex g_mtx[5];

const int thread_num = 5;

class TDel : public _Del {
public:
    virtual void pre_del(const Block *p) {
        PUMP_CORE_INFO("call [yangzheng] pre_del");
    }
    virtual void post_del(size_t data) {
        PUMP_CORE_INFO ("[Del] %s",data);
    }
    TDel(size_t a) {
        data_ = a;
    }
};

void create(size_t i) {
    size_t indx = 0;
    g_mtx[i].Lock();
    if ((indx = g_container[i].size()) > 100) {
        g_mtx[i].Unlock();
#if (defined __linux__)
        sleep(2);
#elif (defined WIN32)
        Sleep(200);
#endif // __linux__
        return;
    }
    g_container[i].push_back(VoidSPtr((rand() % 500), PUMP_NULLPTR, TDel(indx)));
    g_mtx[i].Unlock();
}

void destroy(size_t i) {
    g_mtx[i].Lock();
    if (g_container[i].size()<thread_num) {
        g_mtx[i].Unlock();
#if (defined __linux__)
        sleep(2);
#elif (defined WIN32)
        Sleep(200);
#endif // __linux__
        return;
    }
    g_container[i].pop_front();
    g_mtx[i].Unlock();
}

}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase005, PumpMemoryUnitTestScene000, 
private:
    const int m_nBatch = 10000;
)
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase005 smoke_test1");
    list<VoidSPtr> container;
    for (int i = 0; i < m_nBatch; ++i) {
        for (int i = 0; i<nm_test_VoidSPtr::thread_num; i++) {
            container.push_back(VoidSPtr((rand() % 1000), PUMP_NULLPTR, nm_test_VoidSPtr::TDel(container.size())));
        }
        for (int i = 0; i<nm_test_VoidSPtr::thread_num; i++) {
            container.pop_front();
        }
    }
    PTEST_ASSERT((container.size() == 0), "[ ERROR ] 1-1");
    return 0;
}

class CTestProductorThread : public CThread
{
public:
    virtual pump_int32_t Stop()
    {
        m_bStop = PUMP_TRUE;
        return 0;
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_bStop = PUMP_FALSE;
        while (!m_bStop) {
            nm_test_VoidSPtr::create(0);
            nm_test_VoidSPtr::create(1);
            nm_test_VoidSPtr::create(2);
            nm_test_VoidSPtr::create(3);
            nm_test_VoidSPtr::create(4);
        }
        return 0;
    }
private:
    pump_bool_t m_bStop;
};

class CTestComsumerThread : public CThread
{
public:
    virtual pump_int32_t Stop()
    {
        m_bStop = PUMP_TRUE;
        return 0;
    }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        m_bStop = PUMP_FALSE;
        while (!m_bStop) {
            nm_test_VoidSPtr::destroy(0);
            nm_test_VoidSPtr::destroy(1);
            nm_test_VoidSPtr::destroy(2);
            nm_test_VoidSPtr::destroy(3);
            nm_test_VoidSPtr::destroy(4);
        }
        return 0;
    }
private:
    pump_bool_t m_bStop;
};

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase006, PumpMemoryUnitTestScene000,)
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase006 smoke_test2");
    std::vector<CTestProductorThread *> vecProductorThread;
    std::vector<CTestComsumerThread *> vecComsumerThread;
    for (int i = 0; i<nm_test_VoidSPtr::thread_num; i++) {
        CTestProductorThread * thx = new CTestProductorThread();
        thx->Start();
        vecProductorThread.push_back(thx);
    }
    for (int i = 0; i<nm_test_VoidSPtr::thread_num * 2; i++) {
        CTestComsumerThread * thx = new CTestComsumerThread();
        thx->Start();
        vecComsumerThread.push_back(thx);
    }
    PUMP_CORE_Sleep(10000);
    for (size_t i = 0; i < vecProductorThread.size();i++)
    {
        vecProductorThread[i]->Stop();
    }
    for (size_t i = 0; i < vecComsumerThread.size(); i++)
    {
        vecComsumerThread[i]->Stop();
    }
    return 0;
}

/////////////////////////////////////
//    </>VoidSPtr TestCase</>
/////////////////////////////////////

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase007, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase007 test_VoidSPtr");
    NullPtr null;
    VoidSPtr wsptr_0(PUMP_NULLPTR);
    PTEST_ASSERT(wsptr_0.get() == NULL, "[ ERROR ] 1-1");
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase008, PumpMemoryUnitTestScene000, 
private:
    const int m_nBatch = 1000;
)
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase008 smoke_test_VoidSPtr");
    for (int i = 0; i < m_nBatch;++i) {
        PTEST_G_FN_CALL(TEST_VoidSPtr_1);
    }
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase009, PumpMemoryUnitTestScene000,)
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase009 test_SharedPtr");
    SharedPtr<int> sp_0(PUMP_NULLPTR);
    sp_0.construct(2);
    sp_0.construct(2);
    PTEST_ASSERT((sp_0.ref() == 2), "[ ERROR ] 1-1");
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase010, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase010 test_WeakPtr");
    SharedPtr<int> sp_0(PUMP_NULLPTR);
    sp_0.construct(2);
    WeakPtr<int> wp_0;
    PTEST_ASSERT(wp_0.expired(), "[ ERROR ] 1-1");
    wp_0 = sp_0;
    PTEST_ASSERT(!wp_0.expired(), "[ ERROR ] 1-2");
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase011, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase011 test_trie");
    trie<int> t;
    // insert dictionary key.
    t.insert("proc/pid/mem", 10298);
    t.insert("proc/pid/meminfo", 2);
    t.insert("proc/pid/smaps", 3);
    t.insert("proc/pid/maps", 41);
    PTEST_LOG(log, "----print tree elements----");
    t.print_all();
    //search test
    PTEST_LOG(log, "---search test---");
    trie<int>::iterator it1 = t.find("proc/pid/mem");
    PTEST_ASSERT(!it1.is_null(), "unexist \"proc/pid/mem\"");
    PUMP_CORE_INFO("exist \"proc/pid/mem\" , elem: %d", (*it1).elem);

    trie<int>::iterator it2 = t.find("proc/pid/maps");
    PTEST_ASSERT(!it2.is_null(), "unexist \"proc/pid/maps\"");
    PUMP_CORE_INFO("exist \"proc/pid/maps\", elem: %d", (*it2).elem);

    PTEST_LOG(log, "----delete test----");
    bool isDel = t.remove("proc/pid/mem");
    PTEST_ASSERT(isDel, "delete \"proc/pid/mem\" failed");
    PTEST_LOG(log, "----print tree elements----");
    t.print_all();
    PTEST_LOG(log, "----print all \"proc/pid/s*\"----");
    t.print_pre("proc/pid/s");
    return 0;
}

PTEST_C_CASE_DEF(PumpMemoryUnitTestCase012, PumpMemoryUnitTestScene000, )
{
    PTEST_LOG(comment, "PumpMemoryUnitTestCase012 test_smart_buffer");
    const char * szMsg = "AAAAAAAAAAAAAAAAAAAAAAAAA";
    smart_buffer<char> buf(8, NULL);
    buf.append(szMsg, strlen(szMsg));
    PTEST_ASSERT(buf.size() == strlen(szMsg), "buf.size() != strlen(szMsg)");
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
    return getchar();
PTEST_MAIN_END

