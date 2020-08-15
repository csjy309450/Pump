#include <iostream>
#include <windows.h>
#include <thread>

using namespace std;

typedef DWORD(CALLBACK *pfnReadCB)(const char* pBuf, DWORD dwSize, void* data);

class COuterTerminalPipeServer;

class CInnerTerminalPipe
{
private:
    HANDLE hpiperead = NULL;
    HANDLE hpipewrite = NULL;
    HANDLE hpiperead2 = NULL;
    HANDLE hpipewrite2 = NULL;
    HANDLE hProHandle = NULL;
    HANDLE hThrLoop = NULL;
    HANDLE hThrisLoop = NULL;
    SECURITY_ATTRIBUTES ai;
    PROCESS_INFORMATION pi;
    STARTUPINFOA si;
    BOOL pipe = false;
    INT status = 1;  // 0 = �쳣 1 = ���� -1 = ���� ��
    string errorString;
    pfnReadCB m_pfn_read_cb_;
    COuterTerminalPipeServer *m_pOuterPip;
public:
    void loop();
    void isloop();
    const BOOL isPipeOff() const;
    const INT getStatus() const;
    const string & getError() const;
    const BOOL sendCommand(const char *);
    void setPipeOn(const BOOL);
    void setStatus(const INT, const char*);
    void setStatus(const INT);
    void setReadCB(pfnReadCB pfn, COuterTerminalPipeServer * p);

    CInnerTerminalPipe(char * str);
    ~CInnerTerminalPipe();
};

DWORD __stdcall ThrPipeThreadRead(void *www)
{
    CInnerTerminalPipe * pipe = (CInnerTerminalPipe *)www;
    pipe->loop();
    return 0;
    //�����ں˶���ʹ����֮��һ���ǵùرգ��п��ܻ�����ڴ�й¶
}
DWORD __stdcall WaitPipe(void *www)
{
    CInnerTerminalPipe * pipe = (CInnerTerminalPipe *)www;
    pipe->isloop();
    return 0;
}

CInnerTerminalPipe::CInnerTerminalPipe(char * com)
    : m_pfn_read_cb_(NULL)
{
    ai.nLength = sizeof(SECURITY_ATTRIBUTES);
    ai.bInheritHandle = true;
    ai.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hpiperead, &hpipewrite, &ai, 0))  //��������ܵ�
    {
        this->setStatus(-1, "[0x01]Read ������ʧЧ");
        return;
    }

    if (!CreatePipe(&hpiperead2, &hpipewrite2, &ai, 0))  //��������ܵ�
    {
        this->setStatus(-1, "[0x02]Write ������ʧЧ");
        return;
    }
    GetStartupInfoA(&si);    //��ȡ��ǰ���̵�STARTUPINFO
    si.cb = sizeof(STARTUPINFO);
    si.hStdError = hpipewrite;
    si.hStdOutput = hpipewrite;
    si.hStdInput = hpiperead2;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    if (!(CreateProcessA(NULL, com, NULL, NULL, true, NULL, NULL, NULL, &si, &pi)))      //�������ص�CMD����
    {
        this->setStatus(-1, "[0x03] CreateProcess����ִ�г���");
        return;
    }

    DWORD dwThread = FALSE;
    hThrLoop = CreateThread(NULL, 0, ThrPipeThreadRead, this, 0, &dwThread);//chuangjian
    if (hThrLoop == false) {
        this->setStatus(-1, "[0x11] �̴߳���ʧ�� CreateThread LOOP ʧ��");
        return;
    }
    hThrLoop = CreateThread(NULL, 0, WaitPipe, this, 0, &dwThread);//chuangjian
    if (hThrLoop == false) {
        this->setStatus(-1, "[0x12] �̴߳���ʧ�� CreateThread ISLOOPʧ��");
        return;
    }
}

CInnerTerminalPipe::~CInnerTerminalPipe()
{
    //�����ں˶���ʹ����֮��һ���ǵùرգ��п��ܻ�����ڴ�й¶
    this->setPipeOn(false);
    this->setStatus(-1);
    CloseHandle(hThrisLoop);
    CloseHandle(hThrLoop);
    CloseHandle(hpipewrite);
    CloseHandle(hpiperead);
    CloseHandle(hpiperead2);
    CloseHandle(hpipewrite2);
    CloseHandle(pi.hThread);
    ::TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
}
const INT CInnerTerminalPipe::getStatus() const
{
    return this->status;
}

const string & CInnerTerminalPipe::getError() const
{
    return this->errorString;
}

const BOOL CInnerTerminalPipe::isPipeOff() const
{
    return pipe;
}

void CInnerTerminalPipe::setPipeOn(const BOOL bools)
{
    this->pipe = bools;
}

void CInnerTerminalPipe::setStatus(const INT status, const char * info)
{
    this->errorString = info;    //��˵�᲻���и��õĸ�ֵ����?
    this->status = status;
}

void CInnerTerminalPipe::setStatus(const INT status = 1)
{
    this->status = status;
}

void CInnerTerminalPipe::setReadCB(pfnReadCB pfn, COuterTerminalPipeServer * p)
{
    m_pfn_read_cb_ = pfn;
    m_pOuterPip = p;
}

const BOOL CInnerTerminalPipe::sendCommand(const char * com)     //ִ������
{
    DWORD dwWrite = 0;
    char www[1024];
    memset(www, 0, sizeof(www));
    strcpy_s(www, com);
    strcat_s(www, "\r\n");
    return WriteFile(hpipewrite2, www, strlen(www), &dwWrite, NULL);
    //0x001C7796 ����δ��������쳣(�� ConsoleApplication2.exe ��):  0xC0000005:  ��ȡλ�� 0x0000000C ʱ�������ʳ�ͻ��
}

void CInnerTerminalPipe::loop() {
    char outbuff[4096];        //�������
    DWORD byteread;
    this->setPipeOn(true);
    while (true)
    {
        memset(outbuff, '\0', 4096);
        if (ReadFile(this->hpiperead, outbuff, 4095, &byteread, NULL) == NULL)
        {
            this->setPipeOn(false);
            break;
        }
        // ���

        {
            printf("%s\n", outbuff);
        }
        if (m_pfn_read_cb_)
        {
            m_pfn_read_cb_(outbuff, byteread, m_pOuterPip);
        }
    }
    this->setPipeOn(false);
    std::cout << "Pipe Stoped!" << endl;
}

void CInnerTerminalPipe::isloop()
{
    DWORD dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
    while (dwRet == WAIT_TIMEOUT)
    {
        dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
    }

    if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
    {
        this->setPipeOn(false);
        std::cout << "[END] Pipe Stoped!" << endl;
    }
}

class COuterTerminalPipeServer
{
public:
    COuterTerminalPipeServer()
        : m_pInnerPip(NULL)
        , m_hPipeIn(INVALID_HANDLE_VALUE)
        , m_hPipeOut(INVALID_HANDLE_VALUE)
    {
    }
    ~COuterTerminalPipeServer()
    {
        delete m_pInnerPip;
    }
    int Open()
    {
        int iTime = 0;
        while (1)
        {
            if (WaitNamedPipe("\\\\.\\Pipe\\WinTermIn", NMPWAIT_WAIT_FOREVER) == FALSE)
            {
                if (iTime<1)
                {
                    printf("�ȴ�����ʧ��, %d\n", GetLastError()); iTime++;
                    Sleep(5000);
                    continue;
                }
                return -1;
            }
        // ���Ѵ����Ĺܵ����
        m_hPipeIn = CreateFile("\\\\.\\Pipe\\WinTermIn", GENERIC_READ , 0, NULL, OPEN_EXISTING, NULL, NULL);
        if (m_hPipeIn == INVALID_HANDLE_VALUE)
        {
            printf("�ܵ���ʧ��\n");
            return -1;
        }
        else {
            printf("�ɹ��򿪹ܵ�\n");
        }
        Sleep(1000);
        if (WaitNamedPipe("\\\\.\\Pipe\\WinTermOut", NMPWAIT_WAIT_FOREVER) == FALSE)
        {
            if (iTime < 1)
            {
                printf("�ȴ�����ʧ��, %d\n", GetLastError()); iTime++;
                Sleep(5000);
                continue;
            }
            return -1;
        }
        else
        {
            break;
        }
        }
        m_hPipeOut = CreateFile("\\\\.\\Pipe\\WinTermOut", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
        if (m_hPipeOut == INVALID_HANDLE_VALUE)
        {
            printf("�ܵ���ʧ��\n");
            return -1;
        }
        else {
            printf("�ɹ��򿪹ܵ�\n");
        }
        m_hThx = CreateThread(NULL, 0, __ReadCB, this, 0, &m_dwThxID);
        m_pInnerPip = new CInnerTerminalPipe((char *)"cmd.exe");
        m_pInnerPip->setReadCB(COuterTerminalPipeServer::__WriteCB, this);
        WaitForSingleObject(m_hThx, INFINITE);
        return 0;
    }
private:
    static DWORD __stdcall __ReadCB(void *data)
    {
        COuterTerminalPipeServer * pthis = (COuterTerminalPipeServer*)data;
        char szBuf[1024] = { 0 };
        DWORD ReadNum = 0;
        while (1)
        {
            ReadNum = 0;
            memset(szBuf, 0, sizeof(szBuf));
            if (ReadFile(pthis->m_hPipeIn, szBuf, sizeof(szBuf) - 1, &ReadNum, NULL) == FALSE)
            {
                CloseHandle(pthis->m_hPipeIn); // �رչܵ����
                printf("�ӹܵ���ȡ����ʧ��\n");
                break;
            }
            else {
                //������յ�������
                pthis->__ProcRecvedData(szBuf, ReadNum);
            }
        }
        return 0;
    }
    static DWORD CALLBACK __WriteCB(const char* pBuf, DWORD dwSize, void* data)
    {
        COuterTerminalPipeServer * pthis = (COuterTerminalPipeServer*)data;
        char szBuf[1024] = { 0 };
        DWORD WirteNum = 0;
        if (WriteFile(pthis->m_hPipeOut, pBuf, dwSize, &WirteNum, NULL) == FALSE)
        {
            printf("����д��ܵ�ʧ��, %d\n", GetLastError());
        }
        else {
            printf("���ݳɹ�д��ܵ�\n");
        }
        return WirteNum;
    }
    void __ProcRecvedData(const char* szBuf, DWORD dwSize)
    {
        //m_strCmdCatch += szBuf;
        printf("Recv: %s\n", szBuf);
        if (m_pInnerPip)
        {
            m_pInnerPip->sendCommand(szBuf);
        }
    }
private:
    HANDLE m_hPipeIn;
    HANDLE m_hPipeOut;
    HANDLE m_hThx;
    DWORD m_dwThxID;
    std::string m_strCmdCatch;
    CInnerTerminalPipe *m_pInnerPip;
};

void Read();
void Loop();

CInnerTerminalPipe * pipe;

int Luncher()
{
    pipe = new CInnerTerminalPipe((char *)"cmd.exe");
    thread t1(Read);
    thread t2(Loop);
    t1.join();
    t2.join();
    return 0;
}
void Read() {
    while (true) {
        char str[200];
        memset(str, 0, sizeof(str));
        gets_s(str);
        pipe->sendCommand(str);
    }
}

void Loop() {
    while (true)
    {
        Sleep(1000);
        if (pipe->getStatus() == -11)
        {
            cout << " ERROR " << endl;
            return;
        }
    }
}

//int main(int argc, char *argv[])
//{
//    Luncher();
//    return 0;
//}

int main(int argc, char *argv[])
{
    COuterTerminalPipeServer client;
    client.Open();
    printf("���������\n");
    return getchar();
}