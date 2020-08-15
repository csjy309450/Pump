#ifndef _PUMP_TEST_DEF_H_
#define _PUMP_TEST_DEF_H_
#include <cstdio>
#include <cstring>
#include <cassert>

#ifdef WIN32
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK __cdecl
#endif PUMP_CALLBACK
#else
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#endif // WIN32

/** Global user test prefix, user configured*/
#ifndef PUMP_CONF_TEST_PRE
#define TEST_UNIQUE_PRE_NAME YZ19900322_
#else
#define TEST_UNIQUE_PRE_NAME PUMP_CONF_TEST_PRE
#endif // PUMP_CONF_TEST_PRE

#define PTEST_Printf(_format, ...) CGlobalTestSceneMgr::GetGlobalTestMgr()->OutPut(_format, __VA_ARGS__)

#define PTEST_G_FN_DEF(_indx,...) INTER_PTEST_G_FN_DEF(_indx, __VA_ARGS__)
#define PTEST_G_FN_CALL(_indx,...) INTER_PTEST_G_FN_CALL(_indx, __VA_ARGS__)
#define PTEST_ASSERT(_exp,_format,...) INTER_PTEST_ASSERT(TEST_UNIQUE_PRE_NAME, _exp, _format,__VA_ARGS__)
#define PTEST_LOG(_key,_format,...) PTEST_Printf("  \""#_key"\": "#_format",\n", __VA_ARGS__)
#define PTEST_G_CASE_DEF(_indx) INTER_PTEST_G_FN_DEF(_indx,void)
#define PTEST_G_CASE_RUN_WITH_FN(_indx, _call, _msg) \
do{ \
    PTEST_Printf("{\n  \"test_case\": \""#_indx"\",\n"); \
    if (_call==-1) { \
        PTEST_Printf("  \"result\": \"failed\",\n"); \
        }\
        else { \
        PTEST_Printf("  \"result\": \"succed\",\n"); \
    } \
    PTEST_Printf("  \"msg\": \"%s\"\n}\n", _msg); \
}while(0);
#define TEST_G_CASE_RUN(_indx, _msg) \
do{ \
    PTEST_Printf("{\n  \"test_case\": \""#_indx"\",\n"); \
    if (INTER_MAKE_TEST_FN_NAME(_indx, TEST_UNIQUE_PRE_NAME)()==-1) { \
        PTEST_Printf("  \"result\": \"failed\",\n"); \
        }\
        else { \
        PTEST_Printf("  \"result\": \"succed\",\n"); \
    } \
    PTEST_Printf("  \"msg\": \"%s\"\n}\n", _msg); \
}while(0);
#define INTER_MAKE_STR(X) INTER_MAKE_STR_(X)
#define INTER_MAKE_STR_(X) #X
#define INTER_MAKE_TEST_FN_NAME(_pre, _indx) INTER_MAKE_TEST_FN_NAME_(_pre, _indx)
#define INTER_MAKE_TEST_FN_NAME_(_pre, _indx)  _pre##_indx
#define INTER_PTEST_G_FN_DEF(_indx,_args) INTER_PTEST_G_FN_DEF_(TEST_UNIQUE_PRE_NAME,_indx, _args)
#define INTER_PTEST_G_FN_DEF_(_pre, _indx, _args) int INTER_MAKE_TEST_FN_NAME_(_pre, _indx) (_args)
#define INTER_PTEST_G_FN_CALL(_indx, _args) INTER_PTEST_G_FN_CALL_(TEST_UNIQUE_PRE_NAME,_indx, _args)
#define INTER_PTEST_G_FN_CALL_(_pre, _indx, _args) INTER_MAKE_TEST_FN_NAME_(_pre, _indx)(_args)
#define INTER_PTEST_ASSERT(_pre, _exp, _format,...) INTER_PTEST_ASSERT_(_pre, _exp, _format,__VA_ARGS__)
#define INTER_PTEST_ASSERT_(_pre, _exp, _format,...) \
do{ \
    if(strstr(__FUNCTION__, #_pre)==NULL) {\
        PTEST_Printf("PTEST_ASSERT() must be called in PTEST_G_FN_DEF(...) \n"#_pre); \
        assert(1==0); \
    } \
    if (!_exp) { \
        PTEST_Printf("  \"assert\": {\n    \"pos\": \"%s:%u\",\n    \"msg\": \""#_format"\"\n  }\n", __FILE__,__LINE__,__VA_ARGS__); \
        return -1; \
    }\
}while(0);

#include <list>

namespace Pump
{
namespace Test
{

class CTestCaseBase;
class CTestSceneBase;
class CGlobalTestSceneMgr;

class CTestApplication
{
public:
    CTestApplication();
    ~CTestApplication();
};

class CGlobalTestSceneMgr
{
public:
    CGlobalTestSceneMgr();
    ~CGlobalTestSceneMgr();
    int Run();
    void RegisterTestScene(CTestSceneBase* pScene);
    void OutPut(const char* pFormat, ...);

    static CGlobalTestSceneMgr * GetGlobalTestMgr();

private:
    int (PUMP_CALLBACK *OutPutPUMP_CALLBACK)(const char* pFormat, va_list argv);
private:
    std::list<CTestSceneBase*> m_listTestScene;
    static CGlobalTestSceneMgr * s_pThis;
};

class CTestCaseBase
{
protected:
    CTestCaseBase(const char* pTestCaseId);
    ~CTestCaseBase();
public:
    int Run(CTestSceneBase * pScene);
public:
    virtual int operator()(CTestSceneBase * pScene) = 0;
private:
    const char* m_pTestCaseId;
};

class CTestSceneBase
{
protected:
    CTestSceneBase();
public:
    int Run();
protected:
    void _RegisterTestCase(CTestCaseBase* pCase);
public:
    static CTestSceneBase * s_pThis;
private:
    std::list<CTestCaseBase*> m_listTestCase;
};

}
}

#define  INTER_MAKE_TEST_CLASS_NAME(_pre,_indx) INTER_MAKE_TEST_C_NAME(INTER_MAKE_TEST_C_NAME(_pre,TEST_UNIQUE_PRE_NAME),_indx)
#define INTER_MAKE_TEST_C_NAME(_pre, _indx) INTER_MAKE_TEST_C_NAME_(_pre, _indx)
#define INTER_MAKE_TEST_C_NAME_(_pre, _indx) _pre##_indx

#define PTEST_C_SCENE_DEF(_indx, _code) \
class INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx) \
    : public CTestSceneBase \
{ \
private: \
    INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx)()  \
        : CTestSceneBase() {} \
    ~INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx)(){} \
public: \
    _code \
    static INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx) * GetTestScene() \
    { \
        if (!s_pScene) \
        { \
            s_pScene = new INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx)(); \
        } \
        return s_pScene; \
    } \
    static void RegisterTestCase(CTestCaseBase * pCase) \
    { \
        if (pCase) \
        { \
            GetTestScene()->_RegisterTestCase(pCase); \
        } \
    } \
private: \
    static INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx) * s_pScene; \
};\
INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx) * INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_indx)::s_pScene = NULL;

#define PTEST_C_CASE_DEF(_indx,_scene) \
class INTER_MAKE_TEST_CLASS_NAME(CTestCase_,_indx)  \
    : public CTestCaseBase \
{ \
public: \
    INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx)() \
        : CTestCaseBase(INTER_MAKE_STR(INTER_MAKE_TEST_CLASS_NAME(CTestCase_,_indx))) \
    { \
        INTER_MAKE_TEST_CLASS_NAME(CTestScene_, _scene)::RegisterTestCase(this); \
    } \
    ~INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx)() {} \
    virtual int operator()(CTestSceneBase * pScene); \
private: \
    static  INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx) * s_pCase; \
}; \
INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx) * \
    INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx)::s_pCase \
        = new INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx)(); \
int INTER_MAKE_TEST_CLASS_NAME(CTestCase_, _indx)::operator()(CTestSceneBase * pScene)

#define PTEST_SCENE_PTR(_scene) INTER_MAKE_TEST_CLASS_NAME(CTestScene_,_scene)::GetTestScene()

#define PTEST_MAIN2(_argc, _argv, _code) \
    using namespace Pump::Test; \
    int main(_argc, _argv) \
    {\
        CTestApplication objTest; \
        _code \
    }
#define PTEST_MAIN_BEGINE(_argc, _argv) \
    using namespace Pump::Test; \
    int main(_argc, _argv) \
    {\
        CTestApplication objTest; 
#define PTEST_MAIN_END }

#endif // _PUMP_TEST_DEF_H_
