#include "pump_function/pump_function.hpp"
#include "pump_test/pump_test_def.h"

using namespace Pump::Function;
using namespace Pump::Test;

PTEST_C_SCENE_DEF(FUNCTION_USING_SCAN,
 // TODO 可以定义一些私有成员
 )

class CTest
{
public:
    int TestFn(char*)
    {
        PTEST_LOG(log, "call %s",__FUNCTION__);
        return -1024;
    }
};

CTest fn_obj_arg0_0()
{
    return CTest();
}

CTest & fn_ref_arg1_0(CTest & obj)
{
    return obj;
}

double fn_double_arg2_0(int i, double d) {
    PTEST_LOG(log, "call %s", __FUNCTION__);
    return i+d;
}

int fn_int_arg1_0(char * p) {
    PTEST_LOG(log, "call %s", __FUNCTION__);
    return 1024;
}

int fn_int_arg1_1(CTest obj)
{
    return obj.TestFn(NULL);
}

void fn_void_arg1_0(char * p) {
    PTEST_LOG(log, "call %s", __FUNCTION__);
}

PTEST_C_CASE_DEF(CLOSURE_IT_TEST0, FUNCTION_USING_SCAN)
{
    PTEST_LOG(msg, "%s", "测试各种类型闭包兼容情况");
    CTest objTest;
    CClosure<PUMP_GFN_TYPE(int, (char*))> fn(fn_int_arg1_0, NULL);
    CClosure<PUMP_MFN_TYPE(CTest, int, (char*))> fn_0(&CTest::TestFn, &objTest, NULL);
    CClosure<PUMP_GFN_TYPE(void, (char*))> fn_1(fn_void_arg1_0, NULL);
    CClosure<PUMP_GFN_TYPE(double, (int, double))> fn_2(fn_double_arg2_0, 1, 2.6);
    CClosure<PUMP_GFN_TYPE(CTest, (void))> fn_3(fn_obj_arg0_0);
    fn(); //PTEST_LOG(log, "fn_int_arg1_0() ret=%d", *PUMP_RET_NONVOID(int, &fn, 0));
    fn_0(); //PTEST_LOG(log, "CTest::TestFn() ret=%d", *PUMP_RET_NONVOID(int, &fn_0, 0));
    fn_1(); //PTEST_LOG(log, "CTest::fn_void_arg1_0() ret=%d", PUMP_RET_NONVOID(void, &fn_1, NULL)); 
    fn_2(); //PTEST_LOG(log, "fn_double_arg2_0() ret=%d", *PUMP_RET_NONVOID(double, &fn_2, 0));
    fn_3(); //PTEST_LOG(log, "fn_obj_arg0_0() ret=%d", PUMP_RET_NONVOID(CTest, &fn_3, 0)->TestFn(NULL));
    return 0;
}


PTEST_C_CASE_DEF(CLOSURE_IT_TEST1, FUNCTION_USING_SCAN)
{
    PTEST_LOG(msg, "%s", "测试闭包调用队列情况");
    CTest objTest;
    CClosure<PUMP_GFN_TYPE(int, (char*))> * pfn0 = new  CClosure<PUMP_GFN_TYPE(int, (char*))>(fn_int_arg1_0, NULL);
    CClosure<PUMP_MFN_TYPE(CTest, int, (char*))> * pfn1 = new CClosure<PUMP_MFN_TYPE(CTest, int, (char*))>(&CTest::TestFn, &objTest, NULL);
    CClosure<PUMP_GFN_TYPE(void, (char*))>* pfn2 = new CClosure<PUMP_GFN_TYPE(void, (char*))>(fn_void_arg1_0, NULL);
    //PUMP_GFN(int, (CTest))* pfn3 = new PUMP_GFN(int, (CTest))(fn_int_arg1_1, &objTest);

    std::vector<CClosurePtr> vecFunc;
    vecFunc.push_back(pfn0);
    vecFunc.push_back(pfn1);
    vecFunc.push_back(pfn2);
    //vecFunc.push_back(pfn3);

    for (std::vector<CClosurePtr>::iterator it = vecFunc.begin();
        it != vecFunc.end(); ++it)
    {
        (*(*it))();
        if ((*it)->IsRet())
        {
            PTEST_LOG(log, "ret=%d", (*it)->GetReturn());
        }
    }
    return 0;
}

PTEST_C_CASE_DEF(FUNCTION_IT_TEST1, FUNCTION_USING_SCAN)
{
    PTEST_LOG(msg, "%s", "测试回调对象情况");
    CTest objTest;
    CFunction<PUMP_GFN_TYPE(int, (char*))> fn0(fn_int_arg1_0);
    CFunction<PUMP_MFN_TYPE(CTest, int, (char*))> fn1(&CTest::TestFn, &objTest);
    CFunction<PUMP_GFN_TYPE(void, (char*))> fn2(fn_void_arg1_0);

    int ret0 = fn0(NULL);
    int ret1 = fn1(NULL);
    fn2(NULL);
    return 0;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
{
    return getchar();
}
PTEST_MAIN_END