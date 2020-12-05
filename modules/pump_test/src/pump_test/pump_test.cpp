#include "pump_test/pump_test.h"
#include <cstdarg>

namespace Pump
{
namespace Test
{

CTestApplication::CTestApplication()
{
    CGlobalTestSceneMgr::GetGlobalTestMgr()->Run();
}

CTestApplication::~CTestApplication()
{

}

CGlobalTestSceneMgr * CGlobalTestSceneMgr::s_pThis = NULL;

CGlobalTestSceneMgr::CGlobalTestSceneMgr()
{
    OutPutPUMP_CALLBACK = vprintf;
}

CGlobalTestSceneMgr::~CGlobalTestSceneMgr()
{
    for (std::list<CTestSceneBase*>::iterator it = m_listTestScene.begin();
        it != m_listTestScene.end(); ++it)
    {
        delete (*it);
    }
}

void CGlobalTestSceneMgr::OutPut(const char* pFormat, ...)
{
    va_list argv;
    va_start(argv, pFormat);
    if (OutPutPUMP_CALLBACK)
    {
        OutPutPUMP_CALLBACK(pFormat, argv);
    }
    va_end(argv);
}

int CGlobalTestSceneMgr::Run()
{
    PTEST_Printf(\
        "********************************\n" \
        "* Test Begin\n" \
        "********************************\n" \
        );
    for (std::list<CTestSceneBase*>::iterator it = m_listTestScene.begin();
        it != m_listTestScene.end(); ++it)
    {
        (*it)->Run();
    }
    PTEST_Printf(\
        "********************************\n" \
        "* Test End\n" \
        "********************************\n" \
        );
    return 0;
}

void CGlobalTestSceneMgr::RegisterTestScene(CTestSceneBase* pScene)
{
    m_listTestScene.push_back(pScene);
}

CGlobalTestSceneMgr * CGlobalTestSceneMgr::GetGlobalTestMgr()
{
    if (!s_pThis)
    {
        s_pThis = new (std::nothrow)CGlobalTestSceneMgr();
    }
    return s_pThis;
}

CTestSceneBase * CTestSceneBase::s_pThis = NULL;

CTestSceneBase::CTestSceneBase(const char* pTestSceneId)
    : m_pTestSceneId(pTestSceneId)
{
    CGlobalTestSceneMgr::GetGlobalTestMgr()->RegisterTestScene(this);
}

int CTestSceneBase::Init()
{
    return 0;
}

int CTestSceneBase::Cleanup()
{
    return 0;
}

int CTestSceneBase::Run()
{
    if (Init()==-1)
    {
        PTEST_Printf("#TestScene %s Init failed\n", m_pTestSceneId);
        return -1;
    }
    PTEST_Printf("#TestScene %s Init succed\n", m_pTestSceneId);
    for (std::list<CTestCaseBase*>::iterator it = m_listTestCase.begin();
        it != m_listTestCase.end(); ++it)
    {
        (*it)->Run(this);
    }
    if (Cleanup() == -1)
    {
        PTEST_Printf("#TestScene %s Cleanup failed\n", m_pTestSceneId);
        return -1;
    }
    PTEST_Printf("#TestScene %s Cleanup succed\n", m_pTestSceneId);
    return 0;
}

void CTestSceneBase::_RegisterTestCase(CTestCaseBase* pCase)
{
    m_listTestCase.push_back(pCase);
}

CTestCaseBase::CTestCaseBase(const char* pTestCaseId)
    : m_pTestCaseId(pTestCaseId) {}

CTestCaseBase::~CTestCaseBase() {}

int CTestCaseBase::Init()
{
    return 0;
}

int CTestCaseBase::Cleanup()
{
    return 0;
}

int CTestCaseBase::Run(CTestSceneBase * pScene)
{
    if (Init() == -1)
    {
        PTEST_Printf("#CTestCase %s Init failed\n", m_pTestCaseId);
        return -1;
    }
    PTEST_Printf("#CTestCase %s Init succed\n", m_pTestCaseId);
    PTEST_Printf("{\n  \"test_case\": \"%s\",\n", m_pTestCaseId);
    int ret = (*this)(pScene);
    if (ret == -1) {
        PTEST_Printf("%s", "  \"result\": \"failed\"\n");
    }
    else {
        PTEST_Printf("%s", "  \"result\": \"succed\"\n","");
    }
    PTEST_Printf("%s", "}\n");
    if (Cleanup() == -1)
    {
        PTEST_Printf("#CTestCase %s Cleanup failed\n", m_pTestCaseId);
        return -1;
    }
    PTEST_Printf("#CTestCase %s Cleanup succed\n", m_pTestCaseId);
    return ret;
}

}
}