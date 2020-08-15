#include "pump_test/pump_test_def.h"
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
    for (std::list<CTestSceneBase*>::iterator it = m_listTestScene.begin();
        it != m_listTestScene.end(); ++it)
    {
        (*it)->Run();
    }
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

CTestSceneBase::CTestSceneBase()
{
    CGlobalTestSceneMgr::GetGlobalTestMgr()->RegisterTestScene(this);
}

int CTestSceneBase::Run()
{
    for (std::list<CTestCaseBase*>::iterator it = m_listTestCase.begin();
        it != m_listTestCase.end(); ++it)
    {
        (*it)->Run(this);
    }
    return 0;
}

void CTestSceneBase::_RegisterTestCase(CTestCaseBase* pCase)
{
    m_listTestCase.push_back(pCase);
}

CTestCaseBase::CTestCaseBase(const char* pTestCaseId)
    : m_pTestCaseId(pTestCaseId) {}

CTestCaseBase::~CTestCaseBase() {}

int CTestCaseBase::Run(CTestSceneBase * pScene)
{
    PTEST_Printf("{\n  \"test_case\": \"%s\",\n", m_pTestCaseId);
    int ret = (*this)(pScene);
    if (ret == -1) {
        PTEST_Printf("%s", "  \"result\": \"failed\"\n");
    }
    else {
        PTEST_Printf("%s", "  \"result\": \"succed\"\n","");
    }
    PTEST_Printf("%s", "}\n");
    return ret;
}

}
}