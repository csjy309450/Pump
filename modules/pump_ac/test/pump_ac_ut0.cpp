#include "pump_ac/AC.hpp"
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/pump_ac_document.h"

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
        "    \"key2\" : \"string\"\n"
        "}\n";
    ::Pump::Ac::CDocument doc(::Pump::Ac::PUMP_DOC_JSON);
    doc.parse(strBody1.c_str(), strBody1.size());
    ::Pump::Ac::CNode * pRoot = doc.root();
    ::Pump::Ac::CNode * pSon = pRoot->getFirstSonNode();
    pump_int64_t value = pSon->getValueAsInt();
    printf("type=%d, name=%s, value=%ld\n", pSon->getType(), pSon->getName(), pSon->getValueAsInt());
    return 0;
}

int main(int argc, char**argv)
{
    //test_json_parse();
    test_json_parse2();
    return getchar();
}