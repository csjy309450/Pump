#include <conio.h>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_core/pump_core_api.h"

bool g_stopCThxCmdClient = false;

class CTestCmdClient
    : public ::Pump::Core::Cmder::CCmderClient
{
private:
    virtual pump_void_t ReadCallback(pump_uint32_t iCBType, const char * szBuff, pump_uint32_t dwSize, pump_pvoid_t pdata)
    {
        switch (iCBType)
        {
        case PUMP_CMDER_CLIENT_CB_RECV:
            PUMP_CORE_INFO << "[YZ] CTestCmdClient::ReadCallback() Recv:\n" << szBuff;
            break;
        case PUMP_CMDER_CLIENT_CB_CLOSE:
            PUMP_CORE_INFO << "[YZ] CTestCmdClient::ReadCallback() cmd server closed" << szBuff;
            g_stopCThxCmdClient = true;
            break;
        }
    }
};

class CThxCmdClient
    : public ::Pump::Core::Thread::CThread
{
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        if (m_cmdClient.Open()==PUMP_ERROR)
        {
            PUMP_CORE_ERR << "m_cmdClient.Open() failed";
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
                    PUMP_CORE_ERR << "wirte : " << szCmd[i] << " failed";
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
};

CThxCmdClient g_thxCmdClient;

int test_logger()
{
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    strcpy(struLogCong.szFilePath, "");
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    //PUMP_CORE_InitLogger(&struLogCong);
    PUMP_CORE_INFO << "-------test begin-------";
    return  0;
}

int main(int argc, char** argv)
{
    test_logger();
    if (g_thxCmdClient.Start() != PUMP_OK)
    {
        printf("[YZ] g_thxCmdClient.Start() failed!\n");
        return getchar();
    }
    getchar();
    g_stopCThxCmdClient = true;
    return 0;
}