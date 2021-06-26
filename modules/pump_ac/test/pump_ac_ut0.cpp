#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_app.h"
#include "pump_core/pump_core_logger.h"
#include "pump_ac/pump_ac_document.h"
#include "pump_ac/pump_ac_type.h"
#include "pump_test/pump_test.h"

using namespace ::Pump::Core;
using namespace ::Pump::Ac;
using namespace Pump::Test;

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen);

PTEST_C_SCENE_DEF(PumpAcScene000,
public:
    virtual int Init();
    virtual int Cleanup();
    pump_handle_t GetLogHandle() const;
private:
    pump_handle_t m_hLog = NULL;
    CApplication m_app;
)

int PTEST_SCENE_NAME(PumpAcScene000)::Init()
{
    PTEST_ASSERT((CApplication::IsInit() == PUMP_TRUE), "PUMP_CORE_Init failed 3");
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.pfnLog = Test_CB_WriteLog;
    pump_handle_t hLog = PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_USER);
    PTEST_ASSERT(hLog != PUMP_NULL, "PUMP_CORE_LoggerCreate failed 3");
    PTEST_ASSERT((PUMP_CORE_LoggerConfig(hLog, &struLogCong) == PUMP_OK), "PUMP_CORE_LoggerConfig failed 3");
    PTEST_ASSERT((PUMP_CORE_InjectLocalLogger(hLog) == PUMP_OK), "PUMP_CORE_InjectLocalLogger failed 2");
    m_hLog = hLog;
    return 0;
}
int PTEST_SCENE_NAME(PumpAcScene000)::Cleanup()
{
    PTEST_ASSERT((m_hLog != PUMP_INVALID_HANDLE && m_hLog != NULL), "invalid m_hLog");
    PTEST_ASSERT((PUMP_CORE_LoggerDestroy(m_hLog) == PUMP_OK), "PUMP_CORE_LoggerDestroy failed");
    return 0;
}

pump_handle_t PTEST_SCENE_NAME(PumpAcScene000)::GetLogHandle() const
{
    return m_hLog;
}

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen)
{
    switch (emLogLevel)
    {
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_ERROR:
        PTEST_LOG(log, "[TestLog-E] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_WARNING:
        PTEST_LOG(log, "[TestLog-W] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_INFO:
        PTEST_LOG(log, "[TestLog-I] %s\n", szMessage);
        break;
    }
}

PTEST_C_CASE_DEF(PumpAcUnitTestCase000, PumpAcScene000, )
{
    PTEST_LOG(comment, "PumpAcUnitTestCase000 test network init & cleanup");
    std::string strBody1 =
        "{"
        "    \"key1\" : 12,"
        "    \"key2\" : \"string\","
        "    \"key3\" : {"
        "       \"a\": 1.2"
        "   }"
        "}";
    ::Pump::Ac::CDocument doc(::Pump::Ac::PUMP_DOC_JSON);
    doc.parse(strBody1.c_str(), strBody1.size());
    PTEST_LOG(msg, "%s", doc.dump().c_str());

    for (int i = 0; i < 100; ++i)
    {
        PTEST_LOG(msg, "******* test batch %d **********", i);
        {
            ::Pump::Ac::CNode * pRoot = doc.root();
            ::Pump::Ac::CNode * pRoot2 = doc.root();
            PTEST_ASSERT((pRoot == pRoot2), "CDocument::root() failed.");
            PTEST_ASSERT((pRoot->getType() == ::Pump::Ac::CNode::PUMP_NODE_OBJECT), "pRoot->getType() error.");
            ::Pump::Ac::CNode * key1 = CNode::GetFirstSonNode(pRoot);
            key1->setValueFromInt(1024);
            //PTEST_ASSERT((key1->getValueAsInt() == 1024), "a->getValueAsInt() error.");
            PTEST_ASSERT((key1->getType() == ::Pump::Ac::CNode::PUMP_NODE_INT), "key1->getType() error.");
            PTEST_LOG(msg, "type=%d, name=%s, value=%ld", key1->getType(), key1->getName(), key1->getValueAsInt());
            PTEST_ASSERT((CNode::GetParentNode(key1) == pRoot), "CNode::GetParentNode(key1) error.");
            PTEST_LOG(msg, "GetParentNode succ.");
            PTEST_ASSERT((CNode::GetPreBrother(key1) == NULL), "CNode::GetPreBrother(key1) error.");
            PTEST_LOG(msg, "GetPreBrother succ.");

            CNode * key2 = CNode::GetPostBrother(key1);
            CNode * key3 = CNode::GetPostBrother(key2);
            PTEST_ASSERT((CNode::GetPostBrother(key3) == NULL), "CNode::GetPreBrother(key1) error.");
            PTEST_LOG(msg, "GetPostBrother succ.");

            CNode * a = CNode::GetFirstSonNode(key3);
            PTEST_ASSERT((a->getType() == ::Pump::Ac::CNode::PUMP_NODE_FLOAT), "a->getType() error.");
            PTEST_ASSERT((a->getValueAsFloat() - 1.2 < 0.001), "a->getValueAsInt() error.");
            PTEST_LOG(msg, "type=%d, name=%s, value=%f", a->getType(), a->getName(), a->getValueAsFloat());
            PTEST_LOG(msg, "destroy key3");
            CNode::DestroyNode(key3);
            PTEST_LOG(msg, "destroy root");
            CNode::DestroyNode(pRoot);
        }
    }
    return PUMP_OK;
}

PTEST_C_CASE_DEF(PumpAcUnitTestCase001, PumpAcScene000, )
{
    PTEST_LOG(comment, "PumpAcUnitTestCase001 test network init & cleanup");
    std::string strBody1 =
        "{"
        "    \"key1\" : 12,"
        "    \"key2\" : \"string\","
        "    \"key3\" : {"
        "       \"a\": 1.2"
        "   }"
        "}";
    {
        ::Pump::Ac::CDocument doc(::Pump::Ac::PUMP_DOC_JSON);
        doc.parse(strBody1.c_str(), strBody1.size());
        PTEST_LOG(msg, "%s", doc.dump().c_str());
        ::Pump::Ac::CNode * pRoot = doc.root();
        ::Pump::Ac::CNode * key1 = CNode::GetFirstSonNode(pRoot);
        PTEST_ASSERT((pRoot->getType() == CNode::PUMP_NODE_OBJECT), "root not object");
        const char * szNewNodeName = "NewNode";
        PTEST_ASSERT((pRoot->size() == 3), "size error");
        CNode::CreateSonNodeInteger(pRoot, szNewNodeName, strlen(szNewNodeName), 1024);
        PTEST_LOG(msg, "%s", doc.dump().c_str());
        PTEST_ASSERT((pRoot->size() == 4), "size error");
    }
    return PUMP_OK;
}

PTEST_MAIN_BEGINE(int argc, char** argv)
return getchar();
PTEST_MAIN_END