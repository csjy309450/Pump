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
#include "pump_core/pump_core_api.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_memory/smart_ptr/block.hpp"
#include "pump_memory/smart_ptr/policy.hpp"
#include "pump_memory/smart_ptr/voidsptr.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"
#include "pump_memory/smart_ptr/sharedptr.hpp"
#include "pump_memory/smart_ptr/weakptr.hpp"

using namespace std;
using namespace Pump;
using namespace Pump::Core;
using namespace Pump::Memory;
using namespace Pump::Core::Thread;

bool test_Block() {
    PUMP_CORE_INFO << ">>>>>>>> test_Block <<<<<<<<<";
    Heap<> heap0;
    // test 1
    PUMP_CORE_LOG_ASSERT(heap0.empty()) << "[ OK ] 1-1";
    heap0.allocate(0);
    PUMP_CORE_LOG_ASSERT((heap0.block_segsz_ == 0 && heap0.empty())) << "[ OK ] 1-2";
    heap0.allocate(10);
    PUMP_CORE_LOG_ASSERT((heap0.block_segsz_ == 10 && !heap0.empty())) << "[ OK ] 1-3";
    heap0.deallocate();
    PUMP_CORE_LOG_ASSERT((heap0.block_segsz_ == 0 && heap0.empty())) << "[ OK ] 1-4";

    // test 2
    std::allocator<char> a;
    HeapGuider<> bmgr0;
    HeapGuider<> bmgr1(sizeof(int), a);
    HeapGuider<> bmgr2(sizeof(int));
    PUMP_CORE_LOG_ASSERT((bmgr0.empty())) << "[ OK ] 2-1";
    bmgr1.ref<int>() = 10;
    bmgr2.ref<int>() = 100;
    PUMP_CORE_LOG_ASSERT((!bmgr1.empty() && bmgr1.ref<int>() == 10)) << "[ OK ] 2-2";
    bmgr0 = bmgr1;
    PUMP_CORE_LOG_ASSERT((bmgr0.relative(bmgr1) == RELATIVE_EQUAL)) << "[ OK ] 2-3";
    HeapGuider<> bmgr1_2(bmgr1);
    PUMP_CORE_LOG_ASSERT((bmgr1_2.relative(bmgr1) == RELATIVE_EQUAL)) << "[ OK ] 2-4";
    bmgr1.swap(bmgr2);
    PUMP_CORE_LOG_ASSERT((bmgr1.ref<int>() == 100 && bmgr2.ref<int>() == 10)) << "[ OK ] 2-5";

    return true;
}

class test_del {
public:
    void del() {
        delete(this);
    }
    ~test_del() {
        printf("sss\n");
    }
};

class TDel : public _Del {
public:
    virtual void pre_del(const Block *p) {
        PUMP_CORE_INFO << "call [yangzheng] pre_del";
    }
    void post_del(void * data) {
        PUMP_CORE_INFO << "call [yangzheng] post_del";
    }
};

bool test_VoidSPtr_1() {
    VoidSPtr sp1_1;
    VoidSPtr sp1_2;
    // test 1
    PUMP_CORE_LOG_ASSERT((sp1_1 == PUMP_NULLPTR)
        && (sp1_1.capacity() == 0)
        && (sp1_2 == PUMP_NULLPTR)
        && (sp1_2 != sp1_1)) << "[ OK ] 1-1";
    VoidSPtr sp2(100000);
    PUMP_CORE_LOG_ASSERT((sp2.capacity() == 100000)) << "[ OK ] 1-2";

    // test 2
    TDel d0;
    VoidSPtr sp3(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp4(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp5(100000, PUMP_NULLPTR, d0);
    VoidSPtr sp6(100000, PUMP_NULLPTR, d0);
    PUMP_CORE_LOG_ASSERT((sp3.capacity() == 100000)) << "[ OK ] 1-2";
    sp3.reset();
    return true;
}

bool test_voidwptr() {
    VoidSPtr sp_0(10);
    VoidSPtr sp_1;
    VoidWPtr wp_0;

    PUMP_CORE_LOG_ASSERT((wp_0.expired())) << "[ OK ] 1-1";
    wp_0 = sp_0;
    PUMP_CORE_LOG_ASSERT(!(wp_0.expired())) << "[ OK ] 1-2";
    sp_1 = wp_0.lock_raw();
    PUMP_CORE_LOG_ASSERT(sp_1 == sp_0) << "[ OK ] 1-3";
    VoidWPtr wp_1(sp_1);
    PUMP_CORE_LOG_ASSERT(wp_1 == wp_0) << "[ OK ] 1-4";
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
        PUMP_CORE_INFO << "call [yangzheng] pre_del";
    }
    virtual void post_del(size_t data) {
        PUMP_CORE_INFO << "[Del] " << data;
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

void smoke_test1() {
    list<VoidSPtr> container;
    while (1) {
        for (int i = 0; i<thread_num; i++) {
            container.push_back(VoidSPtr((rand() % 1000), PUMP_NULLPTR, TDel(container.size())));
        }
        for (int i = 0; i<thread_num; i++) {
            container.pop_front();
        }
    }
    getchar();
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
            create(0);
            create(1);
            create(2);
            create(3);
            create(4);
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
            destroy(0);
            destroy(1);
            destroy(2);
            destroy(3);
            destroy(4);
        }
        return 0;
    }
private:
    pump_bool_t m_bStop;
};

void smoke_test2() 
{
    std::vector<CTestProductorThread *> vecProductorThread;
    std::vector<CTestComsumerThread *> vecComsumerThread;
    for (int i = 0; i<thread_num; i++) {
        CTestProductorThread * thx = new CTestProductorThread();
        thx->Start();
        vecProductorThread.push_back(thx);
    }
    for (int i = 0; i<thread_num * 2; i++) {
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
}

}
/////////////////////////////////////
//    </>VoidSPtr TestCase</>
/////////////////////////////////////

bool test_VoidSPtr()
{
    NullPtr null;
    VoidSPtr wsptr_0(PUMP_NULLPTR);
    PUMP_CORE_LOG_ASSERT(wsptr_0.get()==NULL) << "[ OK ] 1-1";
    return true;
}

bool smoke_test_VoidSPtr()
{
    while (1) {
        test_VoidSPtr_1();
    }
    nm_test_VoidSPtr::smoke_test2();
    test_del * a = new test_del;
    return true;
}

bool test_SharedPtr() {
    SharedPtr<int> sp_0(PUMP_NULLPTR);
    sp_0.construct(2);
    sp_0.construct(2);
    PUMP_CORE_LOG_ASSERT(sp_0.ref() == 2) << "[ OK ] 1-1";
    return true;
}

bool test_WeakPtr() {
    SharedPtr<int> sp_0(PUMP_NULLPTR);
    sp_0.construct(2);
    WeakPtr<int> wp_0;
    PUMP_CORE_LOG_ASSERT(wp_0.expired()) << "[ OK ] 1-1";
    wp_0 = sp_0;
    PUMP_CORE_LOG_ASSERT(!wp_0.expired()) << "[ OK ] 1-2";
    return true;
}

int main(int argc, char** argv) {
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    //struLogCong.szFilePath = argv[0];
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    //PUMP_CORE_InitLogger(&struLogCong);
    PUMP_CORE_LOG_ASSERT(test_Block()) << "[ OK ] test_Block()";
    PUMP_CORE_LOG_ASSERT(test_voidwptr()) << "[ OK ] test_voidwptr()";
    PUMP_CORE_LOG_ASSERT(test_SharedPtr()) << "[ OK ] test_SharedPtr()";
    PUMP_CORE_LOG_ASSERT(test_WeakPtr()) << "[ OK ] test_WeakPtr()";
    PUMP_CORE_LOG_ASSERT(test_VoidSPtr()) << "[ OK ] test_VoidSPtr()";

    //smoke_test_VoidSPtr();
    return getchar();
}

