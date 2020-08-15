#include "pump_core/network/pump_core_sock.h"

using namespace Pump;
using namespace Pump::Core::Net;

int main(int argc, char** argv)
{
    PUMP_CORE_InitNetwork();
    CSock *pSock = CSock::CreateSock(PUMP_AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pSock == NULL)
    {
        return -1;
    }
    CAddr objAddr(PUMP_AF_INET, "127.0.0.1", 10002);
    pSock->Connect(objAddr);
    char szBuf[1024] = { 0 };
    while (true)
    {
        memset(szBuf, 0, sizeof(szBuf));
        scanf("%s", szBuf);
        pSock->Send(szBuf, strlen(szBuf));
    }
    return getchar();
}