#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_ac/AC.hpp"
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/pump_ac_document.h"

using namespace ::Pump::Ac;

Json::Value __json_parse(const std::string & strBody)
{
    Json::Reader objReader;
    Json::Value objRoot;
    try
    {
        bool ret = objReader.parse(strBody, objRoot);
        printf("%s is %s\n", strBody.c_str(), (ret ? "good" : "bad"));
    }
    catch (Json::Exception & e)
    {
        printf("%s\n", e.what());
    }
    return objRoot;
}

int test_json_parse()
{
    std::string strBody1 =
        "{\n"
        "    \"key1\" : 1,\n"
        "    \"key2\" : \"string\"\n"
        "}\n";
    __json_parse(strBody1);

    std::string strBody2 =
        "{\n"
        "    \"key1\" : ssss,\n"
        "    \"key2\" : \"string\"\n"
        "}\n";
    __json_parse(strBody2);

    std::string strBody3 =
        "{\n"
        "    \"key1\" : 10000000000000000000000000000000000000000000000000000,\n"
        "    \"key2\" : \"string\"\n"
        "}\n";
    Json::Value objBody3 = __json_parse(strBody3);
    try
    {
        int iNum3 = objBody3["key2"].asInt();
    }
    catch (Json::Exception & e)
    {
        printf("%s\n", e.what());
    }
    
    return 0;
}

int test_json_parse2()
{
    std::string strBody1 =
        "{\n"
        "    \"key1\" : 12,\n"
        "    \"key2\" : \"string\",\n"
        "    \"key3\" : {\n"
        "       \"a\": 1"
        "   }\n"
        "}\n";
    ::Pump::Ac::CDocument doc(::Pump::Ac::PUMP_DOC_JSON);
    doc.parse(strBody1.c_str(), strBody1.size());

    for (int i = 0; ;++i)
    {
        printf("******* test batch %d **********\n", i);
        {
            ::Pump::Ac::CNode * pRoot = doc.root();
            ::Pump::Ac::CNode * pRoot2 = doc.root();
            if (pRoot == pRoot2)
            {
                printf("CDocument::root succ.\n");
            }
            if (pRoot->getType() == ::Pump::Ac::CNode::PUMP_NODE_OBJECT) {
                ::Pump::Ac::CNode * key1 = CNode::GetFirstSonNode(pRoot);
                if (key1->getType() == ::Pump::Ac::CNode::PUMP_NODE_INT)
                {
                    pump_int64_t value = key1->getValueAsInt();
                    printf("type=%d, name=%s, value=%ld\n", key1->getType(), key1->getName(), key1->getValueAsInt());
                }
                if (CNode::GetParentNode(key1) == pRoot)
                {
                    printf("GetParentNode succ.\n");
                }
                if (CNode::GetPreBrother(key1) == NULL)
                {
                    printf("GetPreBrother succ.\n");
                }

                CNode * key2 = CNode::GetPostBrother(key1);
                CNode * key3 = CNode::GetPostBrother(key2);
                if (CNode::GetPostBrother(key3) == NULL)
                {
                    printf("GetPostBrother succ.\n");
                }

                CNode * a = CNode::GetFirstSonNode(key3);
                if (a->getType() == ::Pump::Ac::CNode::PUMP_NODE_INT)
                {
                    pump_int64_t value = a->getValueAsInt();
                    printf("type=%d, name=%s, value=%ld\n", a->getType(), a->getName(), a->getValueAsInt());
                }
                printf("destroy key3\n");
                CNode::DestroyNode(key3);
                printf("destroy root\n");
                CNode::DestroyNode(pRoot);
            }
        }
        //PUMP_CORE_Sleep(500);
        //char * sss = new char[100000];
    }
    return 0;
}

int main(int argc, char**argv)
{
    //test_json_parse();
    test_json_parse2();
    return getchar();
}