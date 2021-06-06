#include <conio.h>
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/pump_core_thread.h"
#include "pump_core/pump_core_app.h"
#include "pump_core/os_wrapper/pump_core_os_api.h"

bool g_stopCThxCmdClient = false;

class CTestCmdClient
    : public ::Pump::Core::Cmder::CCmderClient
{
private:
    virtual pump_void_t ReadCallback(pump_uint32_t iCBType, const char * szBuff, pump_uint32_t dwSize, pump_pvoid_t pdata)
    {
        switch (iCBType)
        {
        case PUMP_CMDER_CLIENT_CB_BEGIN:
            //m_buffer.clear();
            m_buffer = szBuff;
            break;
        case PUMP_CMDER_CLIENT_CB_END:
            m_buffer += szBuff;
            PUMP_CORE_INFO("[YZ] CTestCmdClient::ReadCallback() Recv:\n%s", m_buffer.c_str());
        case PUMP_CMDER_CLIENT_CB_RECV:
            m_buffer += szBuff;
            break;
        case PUMP_CMDER_CLIENT_CB_CLOSE:
            PUMP_CORE_INFO("[YZ] CTestCmdClient::ReadCallback() cmd server closed %s", szBuff);
            g_stopCThxCmdClient = true;
            break;
        }
    }
    std::string m_buffer;
};

class CThxCmdClient
    : public ::Pump::Core::Thread::CThread
{
public:
    CThxCmdClient(const std::string & strPipeName)
        : m_strPipeName(strPipeName)
    {}
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        if (m_cmdClient.Open(m_strPipeName.c_str())==PUMP_ERROR)
        {
            PUMP_CORE_ERR("m_cmdClient.Open() failed");
            return NULL;
        }
        while (!g_stopCThxCmdClient)
        {
            const char * szCmd[] = {
                "cmake -h"
                , "pwd"
                , "cd ../"
                , "\"C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/bin/cl.exe\" /help"
                , "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/bin/cl.exe"
            };
            //char szBuff[128] = { 0 };
            //scanf("%s\r\n", szBuff);
            for (int i = 0; i < sizeof(szCmd)/sizeof(char*);++i)
            {
                int ntime = 0;
                while (m_cmdClient.Write(szCmd[i], strlen(szCmd[i]))!=PUMP_OK)
                {
                    PUMP_CORE_ERR("wirte : %s failed",szCmd[i]);
                    PUMP_CORE_Sleep(100);
                    ntime++;
                    if (ntime==10)
                    {
                        break;
                    }
                }
            }
            PUMP_CORE_Sleep(2000);
        }
        /*{
            char szBuff[128] = { 0 };
            int c = 0, d = 0;
            do
            {
            d = getch();
            if (d != '\r')
            {
            szBuff[c++] = d;
            continue;
            }
            szBuff[c++] = '\r';
            szBuff[c++] = '\n';
            auto fSuccess = m_cmdClient.Write(szBuff, strlen(szBuff));
            if (fSuccess != PUMP_OK)
            {
            printf("write pipe failed:%d\r\n", GetLastError());
            }
            else
            {
            printf("send pipe :%s\r\n", szBuff);
            }
            c = 0; d = 0;
            memset(szBuff, 0, sizeof(szBuff));
            } while (1);
            }*/
        return NULL;
    }
private:
    CTestCmdClient m_cmdClient;
    std::string m_strPipeName;
};

class CThxCmdClient2
    : public ::Pump::Core::Thread::CThread
{
public:
    CThxCmdClient2(const std::string & strPipeName)
        : m_strPipeName(strPipeName)
    {}
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        if (m_cmdClient.Open(m_strPipeName.c_str()) == PUMP_ERROR)
        {
            PUMP_CORE_ERR("m_cmdClient.Open() failed");
            return NULL;
        }
        {
            char szBuff[128] = { 0 };
            int c = 0, d = 0;
            do
            {
                d = getch();
                printf("%c", d);
                if (d != '\r')
                {
                    szBuff[c++] = d;
                    continue;
                }
                szBuff[c++] = '\r';
                szBuff[c++] = '\n';
                auto fSuccess = m_cmdClient.Write(szBuff, strlen(szBuff));
                if (fSuccess != PUMP_OK)
                {
                    printf("write pipe failed:%d\r\n", GetLastError());
                }
                else
                {
                    printf("send pipe :%s\r\n", szBuff);
                }
                c = 0; d = 0;
                memset(szBuff, 0, sizeof(szBuff));
            } while (1);
        }
        return NULL;
    }
private:
    CTestCmdClient m_cmdClient;
    std::string m_strPipeName;
};

int test_logger()
{
    static ::Pump::Core::CApplication app;
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.bPrintConsole = PUMP_TRUE;
    struLogCong.bWriteFile = PUMP_TRUE;
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    strcpy(struLogCong.szFilePath, "yz_log");
    pump_handle_t hLog = PUMP_CORE_LoggerCreate();
    PUMP_CORE_LoggerConfig(hLog, &struLogCong);
    PUMP_CORE_InjectLocalLogger(hLog);
    PUMP_CORE_INFO("-------cmd client-------");
    return  0;
}

int main(int argc, char** argv)
{
    test_logger();
    CThxCmdClient2 g_thxCmdClient("01FD9A84_C2AA_4edb_BAA1_D197E4B5D03E");
    if (g_thxCmdClient.Start() != PUMP_OK)
    {
        printf("[YZ] g_thxCmdClient.Start() failed!\n");
        return getchar();
    }
    while (1) { PUMP_CORE_Sleep(5000); }
    g_stopCThxCmdClient = true;
    return 0;
}