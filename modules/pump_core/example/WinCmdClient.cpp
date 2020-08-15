#include <iostream>
#include <windows.h>
#include <thread>
#include "conio.h"

using namespace std;

typedef DWORD(CALLBACK *pfnReadCB)(const char* pBuf, DWORD dwSize, void* data);

class CTerminalPipeClient
{
public:
    CTerminalPipeClient()
        : m_hPipeOut(INVALID_HANDLE_VALUE)
    {
    }
    ~CTerminalPipeClient()
    {
        CloseHandle(m_hPipeOut);
        CloseHandle(m_hPipeIn);
        m_bStop = false;
        WaitForSingleObject(m_hThx, INFINITY);
    }
    int Open()
    {
        SECURITY_ATTRIBUTES ai;
        ai.nLength = sizeof(SECURITY_ATTRIBUTES);
        ai.bInheritHandle = false;
        ai.lpSecurityDescriptor = NULL;
        m_hPipeOut = CreateNamedPipe("\\\\.\\Pipe\\WinTermIn", 
            PIPE_ACCESS_OUTBOUND ,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000, &ai); // ���������ܵ�
        if (m_hPipeOut == INVALID_HANDLE_VALUE)
        {
            printf("���������ܵ�ʧ��!\n");
            return -1;
        }
        else{
            printf("�ɹ����������ܵ�!\n");
        }
        if (ConnectNamedPipe(m_hPipeOut, NULL) == FALSE) // �ȴ��ͻ���������
        {
            CloseHandle(m_hPipeOut); // �رչܵ����
            printf("��ͻ�����������ʧ��!\n");
            return -1;
        }
        else{
            printf("��ͻ�����������!\n");
        }
        m_hPipeIn = CreateNamedPipe("\\\\.\\Pipe\\WinTermOut", 
            PIPE_ACCESS_INBOUND, 
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000, &ai); // ���������ܵ�
        if (m_hPipeIn == INVALID_HANDLE_VALUE)
        {
            printf("���������ܵ�ʧ��!\n");
            return -1;
        }
        else{
            printf("�ɹ����������ܵ�!\n");
        }
        if (ConnectNamedPipe(m_hPipeIn, NULL) == FALSE) // �ȴ��ͻ���������
        {
            CloseHandle(m_hPipeIn); // �رչܵ����
            printf("��ͻ�����������ʧ��!\n");
            return -1;
        }
        else{
            printf("��ͻ�����������!\n");
        }

        m_hThx = CreateThread(NULL, 0, __ReadCB, this, 0, &m_dwThxID);
        char szBuf[1024] = {0};
        DWORD dwSize = 0;
        const char * pCmd = "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/bin/cl.exe /help\r\n";
        while (1)
        {
            /*char ch = _getch();
            szBuf[dwSize++] = ch;
            printf("%c", ch);
            if (ch == '\r')
            {
            printf("\n");
            DWORD WriteNum = 0;
            if (WriteFile(m_hPipeOut, szBuf, dwSize, &WriteNum, NULL) == FALSE)
            {
            printf("����д��ܵ�ʧ��!\n");
            break;
            }
            else {
            printf("���ݳɹ�д��ܵ�!\n");
            }
            memset(szBuf, 0, sizeof(szBuf));
            dwSize = 0;
            continue;
            }*/
            DWORD WriteNum = 0;
            if (WriteFile(m_hPipeOut, pCmd, strlen(pCmd), &WriteNum, NULL) == FALSE)
            {
                printf("����д��ܵ�ʧ��!\n");
                break;
            }
            else {
                printf("���ݳɹ�д��ܵ�!\n");
            }
            Sleep(2000);
        }
        return 0;
    }
private:
    static DWORD __stdcall __ReadCB(void *data)
    {
        CTerminalPipeClient * pthis = (CTerminalPipeClient*)data;
        char szBuf[1024] = { 0 };
        DWORD ReadNum = 0;
        while (pthis->m_bStop)
        {
            ReadNum = 0;
            memset(szBuf, 0, sizeof(szBuf));
            if (ReadFile(pthis->m_hPipeIn, szBuf, sizeof(szBuf) - 1, &ReadNum, NULL) == FALSE)
            {
                CloseHandle(pthis->m_hPipeIn); // �رչܵ����
                printf("�ӹܵ���ȡ����ʧ��\n");
            }
            else {
                //������յ�������
                printf("%s", szBuf);
            }
        }
        return 0;
    }
private:
    HANDLE m_hPipeOut;
    HANDLE m_hPipeIn;
    HANDLE m_hThx;
    DWORD m_dwThxID;
    std::string m_strCmdCatch;
    bool m_bStop;
};

int main(int argc, char *argv[])
{
    CTerminalPipeClient client;
    client.Open();
    printf("���������\n");
    return getchar();
}