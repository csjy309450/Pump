#include <map>
#include <utility>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_core/network/pump_core_multiplex.h"

using namespace Pump;
using namespace Pump::Core::Net;

int test_server_0()
{
    CSock *pSock = CSock::CreateSock(PUMP_AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pSock == NULL)
    {
        return -1;
    }
    CAddr objAddr(PUMP_AF_INET, "127.0.0.1", 10002);
    pSock->Bind(objAddr);
    pSock->Listen(5);
    while (1)
    {
        CSock * pClientSock = pSock->Accept();
        printf("client ����\n");
        pClientSock->SetNonBlock(PUMP_TRUE);
        char szBuf[10] = { 0 };
        pump_bool_t bRun = PUMP_TRUE;
        while (bRun)
        {
            while (1)
            {
                memset(szBuf, 0, sizeof(szBuf));
                int ret = pClientSock->Recv(szBuf, sizeof(szBuf) - 1);
                if (ret == PUMP_ERROR)
                {
                    pump_bool_t outRecvLoop = PUMP_FALSE;
                    switch (PUMP_CORE_GetSystemError())
                    {
                    case WSAEWOULDBLOCK:
                        printf("\nclient ����������");
                        outRecvLoop = PUMP_TRUE;
                        ::Sleep(1000);
                        break;
                    case 10054:
                        outRecvLoop = PUMP_TRUE;
                        printf("client �Ͽ�����");
                        pClientSock->Close();
                        CSock::DestroySock(pClientSock);
                        bRun = PUMP_FALSE;
                        break;
                    }
                    if (outRecvLoop)
                    {
                        break;
                    }
                }
                else if (ret == 0)
                {
                    printf("client �Ͽ�����");
                    pClientSock->Close();
                    CSock::DestroySock(pClientSock);
                    bRun = PUMP_FALSE;
                }
                else
                {
                    printf("%s", szBuf);
                }
            }
            printf("\n");
        }
    }
    return 0;
}

int test_server_1()
{
    std::map<pump_fd_t, CSock *> mapSockList;
    CSock *pSock = CSock::CreateSock(PUMP_AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pSock == NULL)
    {
        return -1;
    }
    mapSockList.insert(std::pair<pump_fd_t, CSock *>((pump_fd_t)pSock->GetSock(), pSock));
    CAddr objAddr(PUMP_AF_INET, "127.0.0.1", 10002);
    pSock->ListenExt(objAddr, 5);
    CSelect objSelect;
    CFdCtl objFdCtrl;
    objFdCtrl.fd_ = (pump_fd_t)pSock->GetSock();
    objFdCtrl.fd_ev_ = PUMP_IOEV_IN;
    objFdCtrl.type_ = PUMP_FDCTL_ADD;
    objSelect.update(objFdCtrl);
    CFdRetList objFdRetList;
    timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    char szBuf[1024];
    while (1)
    {
        objFdRetList.clear();
        objSelect.wait(objFdRetList, tv);
        for (CFdRetList::iterator it = objFdRetList.begin(); it != objFdRetList.end();++it)
        {
            if (((*it).re_fd_ev_ | PUMP_IOEV_IN) !=0)
            {
                std::map<pump_fd_t, CSock *>::iterator itSock = mapSockList.find((*it).fd_);
                if (itSock != mapSockList.end())
                {
                    if (((*itSock).second)->GetSockState() == CSockState::PUMP_SOCK_LISTENED)
                    {
                        CSock *pClientSock = pSock->Accept();
                        objFdCtrl.fd_ = (pump_fd_t)pClientSock->GetSock();
                        objFdCtrl.fd_ev_ = PUMP_IOEV_IN | PUMP_IOEV_ERR;
                        objFdCtrl.type_ = PUMP_FDCTL_ADD;
                        objSelect.update(objFdCtrl);
                        mapSockList.insert(std::pair<pump_fd_t, CSock *>((pump_fd_t)pClientSock->GetSock(), pClientSock));
                    }
                    else  if (((*itSock).second)->GetSockState() == CSockState::PUMP_SOCK_CONNECTED)
                    {
                        memset(szBuf, 0, sizeof(szBuf));
                        int ret = ((*itSock).second)->Recv(szBuf, sizeof(szBuf) - 1);
                        if (ret==-1)
                        {
                            pump_uint32_t dwErr = PUMP_CORE_GetSystemError();
                            switch (dwErr)
                            {
                            case 10054:
                            {
                                objFdCtrl.fd_ = (pump_fd_t)(*itSock).first;
                                objFdCtrl.type_ = PUMP_FDCTL_DEL;
                                objSelect.update(objFdCtrl);
                                ((*itSock).second)->Close();
                                CSock::DestroySock(((*itSock).second));
                                mapSockList.erase(itSock);
                                printf("�Ͽ�����\n");
                            } break;
                            default:
                            {
                                printf("��������ʧ��\n");
                            }
                            }
                        }
                        else
                        {
                            printf("%s\n",szBuf);
                        }
                    }
                }
                printf("PUMP_IOEV_IN\n");
            }
            else if (((*it).re_fd_ev_ | PUMP_IOEV_OUT) != 0)
            {
                printf("PUMP_IOEV_OUT\n");
            }
            else if (((*it).re_fd_ev_ | PUMP_IOEV_ERR) != 0)
            {
                printf("PUMP_IOEV_ERR\n");
            }
        }
    }
    
    return 0;
}

int main(int argc, char** argv)
{
    PUMP_CORE_InitNetwork();
    //test_server_0();
    test_server_1();
    return getchar();
}