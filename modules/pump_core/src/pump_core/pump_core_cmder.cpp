/**
 * @file pump_core_cmder.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of CMD control interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/pump_core_string.h"
#include "pump_core/pump_core_uuid.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/__pump_core_global_ctrl.h"
//#include "pump_core/deelx.h"

namespace Pump
{
namespace Core
{
namespace Cmder
{
#if defined PUMP_OS_WINDOWS
const char* kCmdNamePipeIn = "\\\\.\\Pipe\\WinTermIn"; ///< Pipe name add uuid suffix for supporting multi terminal.
const char* kCmdNamePipeOut = "\\\\.\\Pipe\\WinTermOut";
const char* kWinCmdx64 = "C:\\Windows\\SysWOW64\\cmd.exe";
const char* kWinCmdx32 = "C:\\Windows\\System32\\cmd.exe";

CCmderClient::CCmderClient()
    : m_bOpen(PUMP_FALSE)
    , m_bAvailable(PUMP_FALSE)
    , m_pHPipeIn(PUMP_NULL)
    , m_pHPipeOut(PUMP_NULL)
    , m_thxRead(this)
{
}

CCmderClient::~CCmderClient()
{
    this->Close();
}

pump_int32_t CCmderClient::Open()
{
    std::string strPipeName, strUUID = __CPumpCoreGlobalCtrl::GetCmdSessionMgr() ? __CPumpCoreGlobalCtrl::GetCmdSessionMgr()->CmdServerGetUUID() : "";
    if (strUUID.empty())
    {
        return PUMP_ERROR;
    }
    strPipeName = kCmdNamePipeIn + strUUID;
    m_pHPipeIn = CPipeHandle::CreateNamedPipeClient(strPipeName.c_str(), CPipeHandle::PUMP_PIPE_READ);
    if (!m_pHPipeIn)
    {
        return PUMP_ERROR;
    }

    strPipeName = kCmdNamePipeOut + strUUID;
    m_pHPipeOut = CPipeHandle::CreateNamedPipeClient(strPipeName.c_str(), CPipeHandle::PUMP_PIPE_WRITE);
    if (!m_pHPipeOut)
    {
        CPipeHandle::DestroyPipe(m_pHPipeIn);
        m_pHPipeIn = PUMP_NULL;
        return PUMP_ERROR;
    }
    //Sleep(5000);
    m_thxRead.SetData(this);
    m_thxRead.Start();
    m_bOpen = PUMP_TRUE;
    return PUMP_OK;
}

pump_int32_t CCmderClient::Close()
{
    // close client read thread.
    m_bOpen = PUMP_FALSE;
    m_thxRead.Stop();
    // close pipe.
    CPipeHandle::DestroyPipe(m_pHPipeIn);
    CPipeHandle::DestroyPipe(m_pHPipeOut);
    //m_thxRead.Wait();
    return PUMP_OK;
}

pump_int32_t CCmderClient::Write(const char * szBuff, pump_uint32_t dwSize)
{
    if (!m_bAvailable)
    {
        return PUMP_ERROR;
    }
    if (m_state.GetState() == PUMP_FALSE)
    { // 
        return PUMP_ERROR;
    }
    m_strLastCmd = szBuff;
    if (m_strLastCmd[m_strLastCmd.size() - 2] != '\r' 
        || m_strLastCmd[m_strLastCmd.size() - 1] != '\n')
    {
        m_strLastCmd += "\r\n";
    }
    pump_int32_t ret = m_pHPipeOut->Write(m_strLastCmd.c_str(), m_strLastCmd.size(), NULL);
    m_state.SetState(PUMP_FALSE);
    return ret;
}

pump_void_t CCmderClient::ReadCallback(pump_uint32_t iCBType, const char * szBuff, pump_uint32_t dwSize, pump_pvoid_t pdata)
{
    
}

CCmderClient::CThxRead::CThxRead(pump_pvoid_t pdata)
    : ::Pump::Core::Thread::CThread(this)
{}

pump_void_t * CCmderClient::CThxRead::ThreadCallback(pump_void_t * pData)
{
    CCmderClient * pClient = (CCmderClient*)pData;
    char szBuf[1024] = { 0 };
    pump_uint32_t ReadNum = 0;
    std::string strBuff;
    PUMP_CMDER_CLIENT_CB_TYPE emLastState;
    while (pClient->m_bOpen)
    {
        ReadNum = 0;
        memset(szBuf, 0, sizeof(szBuf));
        if (pClient->m_pHPipeIn->Read(szBuf, sizeof(szBuf) - 1, &ReadNum) == PUMP_ERROR)
        {
            pClient->m_pHPipeIn->Close();
            emLastState = PUMP_CMDER_CLIENT_CB_CLOSE;
            strBuff = PUMP_CORE_StringTrim(strBuff);
            pClient->ReadCallback(PUMP_CMDER_CLIENT_CB_CLOSE, strBuff.c_str(), strBuff.size(), NULL);
            PUMP_CORE_ERR << "CCmderClient::CThxRead::ThreadCallback() fail to read data from pipe";
            break;
        }
        else {
            if (ReadNum==0)
            {
                if (emLastState == PUMP_CMDER_CLIENT_CB_END)
                {
                    pClient->m_state.SetState(PUMP_TRUE);
                    strBuff = PUMP_CORE_StringTrim(strBuff);
                    pClient->ReadCallback(emLastState, strBuff.c_str(), strBuff.size(), NULL);
                    strBuff.clear();
                    emLastState = PUMP_CMDER_CLIENT_CB_OPEN;
                }
                continue;
            }
            // handle received data
            if (!pClient->m_bAvailable)
            {
                if (strstr(szBuf, "(c) 2018 Microsoft Corporation"))
                {
                    pClient->m_bAvailable = PUMP_TRUE;
                    emLastState = PUMP_CMDER_CLIENT_CB_OPEN;
                    strBuff = PUMP_CORE_StringTrim(strBuff);
                    pClient->ReadCallback(emLastState, strBuff.c_str(), strBuff.size(), NULL);
                }
            }
            else
            {
                if (szBuf[ReadNum-1]=='>') // windows cmd output ">" symbolized the end.
                {
                    strBuff = szBuf;
                    // Get rid of command line string.
                    if (emLastState == PUMP_CMDER_CLIENT_CB_OPEN)
                    {
                        std::string::size_type pos = strBuff.find((pClient->m_strLastCmd).c_str());
                        if (pos == 0)
                        {
                            strBuff = strBuff.substr(pos + pClient->m_strLastCmd.size(), strBuff.size() - pClient->m_strLastCmd.size());
                        }
                        pClient->ReadCallback(PUMP_CMDER_CLIENT_CB_BEGIN, pClient->m_strLastCmd.c_str(), pClient->m_strLastCmd.size(), NULL);
                    }
                    // Get rid of the line with "...>"
                    std::string::size_type pos = strBuff.find_last_of("\r\n");
                    if (pos != std::string::npos)
                    {
                        strBuff = strBuff.substr(0, pos);
                    }
                    //PUMP_CORE_INFO << "CCmderClient::Recv: \n" << strMsg.c_str();
                    emLastState = PUMP_CMDER_CLIENT_CB_END;
                    continue;
                }
                else
                {
                    strBuff = szBuf;
                    if (emLastState == PUMP_CMDER_CLIENT_CB_OPEN)
                    {
                        emLastState = PUMP_CMDER_CLIENT_CB_BEGIN;
                        std::string::size_type pos = strBuff.find((pClient->m_strLastCmd).c_str());
                        if (pos != std::string::npos)
                        {
                            strBuff = strBuff.substr(pos, strBuff.size() - pClient->m_strLastCmd.size());
                        }
                        pClient->ReadCallback(PUMP_CMDER_CLIENT_CB_BEGIN, pClient->m_strLastCmd.c_str(), pClient->m_strLastCmd.size(), NULL);
                    }
                    strBuff = PUMP_CORE_StringTrim(strBuff);
                    pClient->ReadCallback(PUMP_CMDER_CLIENT_CB_RECV, strBuff.c_str(), strBuff.size(), NULL);
                    strBuff.clear();
                }
            }
        }
    }
    return PUMP_NULL;
}

CCmderServer::__CInnerCmdService::__CInnerCmdService()
    : m_pfnReadCB(NULL)
    , m_pHPipeIn(PUMP_NULL)
    , m_pHPipeOut(PUMP_NULL)
    , m_hProcHandle(NULL)
    , m_hThxWaitExit(NULL)
    , m_hThxRead(NULL)
    , m_pOuterPip(PUMP_NULL)
{
    ::memset(&m_struProcInfo, 0, sizeof(m_struProcInfo));
    ::memset(&m_struStartupInfo, 0, sizeof(m_struStartupInfo));
}

CCmderServer::__CInnerCmdService::~__CInnerCmdService()
{
    // / close thread first, then close pipe.
    this->Close();
    ::CloseHandle(m_hThxRead);
    ::CloseHandle(m_hThxWaitExit);
    // close pipe.
    CPipeHandle::DestroyPipe(m_pHPipeIn);
    CPipeHandle::DestroyPipe(m_pHPipeOut);
    // close terminal process
    ::CloseHandle(m_struProcInfo.hThread); 
    ::TerminateProcess(m_struProcInfo.hProcess, 0);
    //char cmdData[128] = { 0 };
    //sprintf_s(cmdData, "taskkill /F /PID %d /T", m_struProcInfo.dwProcessId);
    //system(cmdData);
    ::CloseHandle(m_struProcInfo.hProcess);
}

DWORD __stdcall  CCmderServer::__CInnerCmdService::__ThxReadCB(void *pData)
{
    CCmderServer::__CInnerCmdService * pCmd = (CCmderServer::__CInnerCmdService *)pData;
    pCmd->Loop();
    return 0;
}
DWORD __stdcall  CCmderServer::__CInnerCmdService::__ThxWaitExitCB(void *pData)
{
    CCmderServer::__CInnerCmdService * pCmd = (CCmderServer::__CInnerCmdService *)pData;
    pCmd->IsLoop();
    return 0;
}

pump_int32_t CCmderServer::__CInnerCmdService::Open()
{
    m_pHPipeIn = CPipeHandle::CreateAnonyPipe();
    if (!m_pHPipeIn)  // create in pipe
    {
        PUMP_CORE_ERR << "fail to create read pipe";
        return PUMP_ERROR;
    }
    m_pHPipeOut = CPipeHandle::CreateAnonyPipe();
    if (!m_pHPipeOut)  // create out pipe
    {
        PUMP_CORE_ERR << "fail to create write pipe";
        CPipeHandle::DestroyPipe(m_pHPipeIn);
        m_pHPipeIn = PUMP_NULL;
        return PUMP_ERROR;
    }

    CPipeHandle::LPPUMP_RAW_PIPE pRawPIpeIn = (CPipeHandle::LPPUMP_RAW_PIPE)m_pHPipeIn->GetHandle();
    CPipeHandle::LPPUMP_RAW_PIPE pRawPIpeOut = (CPipeHandle::LPPUMP_RAW_PIPE)m_pHPipeOut->GetHandle();
    ::GetStartupInfoA(&m_struStartupInfo);    // Get STARTUPINFO of current process
    m_struStartupInfo.cb = sizeof(STARTUPINFO);
    m_struStartupInfo.hStdError = pRawPIpeIn->arrAnonyPipe_[1];
    m_struStartupInfo.hStdOutput = pRawPIpeIn->arrAnonyPipe_[1];
    m_struStartupInfo.hStdInput = pRawPIpeOut->arrAnonyPipe_[0];
    m_struStartupInfo.wShowWindow = SW_HIDE;
    m_struStartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!(::CreateProcessA(NULL, (char*)kWinCmdx64, NULL, NULL, true, NULL, NULL, NULL, &m_struStartupInfo, &m_struProcInfo)))      //创建隐藏的CMD进程
    {
        PUMP_CORE_ERR << "CreateProcess() run failed";
        return PUMP_ERROR;
    }

    DWORD dwThread = FALSE;
    m_hThxRead = ::CreateThread(NULL, 0, __ThxReadCB, this, 0, &dwThread);
    if (m_hThxRead == false) {
         PUMP_CORE_ERR << "create __ThxReadCB thread failed";
        return PUMP_ERROR;
    }
    m_hThxWaitExit = ::CreateThread(NULL, 0, __ThxWaitExitCB, this, 0, &dwThread);
    if (m_hThxWaitExit == false) {
        PUMP_CORE_ERR << "create __ThxWaitExitCB thread failed";
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CCmderServer::__CInnerCmdService::Close()
{
    m_bStopThxRead = PUMP_TRUE;
    ::WaitForSingleObject(m_hThxRead, INFINITE); // must close thread first then close pipe, otherwise, may cause crash.
    return PUMP_OK;
}

void CCmderServer::__CInnerCmdService::SetReadCB(fnOnReadCB pfn, CCmderServer * p)
{
    m_pfnReadCB = pfn;
    m_pOuterPip = p;
}

/**
 * @brief run user cmd
 */
pump_int32_t CCmderServer::__CInnerCmdService::InputCommand(const char * pCmd)
{
    pump_uint32_t dwWrite = 0;
    char szBuff[1024];
    memset(szBuff, 0, sizeof(szBuff));
    strcpy_s(szBuff, pCmd);
    //strcat_s(szBuff, "\r\n");
    return m_pHPipeOut->Write(szBuff, strlen(szBuff), &dwWrite);
}

void CCmderServer::__CInnerCmdService::Loop() 
{
    char outbuff[4096];        //output buffer 
    pump_uint32_t byteread;
    while (!m_bStopThxRead)
    {
        memset(outbuff, '\0', 4096);
        if (PUMP_ERROR == m_pHPipeIn->Read(outbuff, sizeof(outbuff) - 1, &byteread))
        {
            if (byteread==0)
            {
                m_pHPipeIn->Close();
                PUMP_CORE_INFO << "__CInnerCmdService::Loop() remote pipe close";
                break;
            }
            else
            {
                m_pHPipeIn->Close();
                PUMP_CORE_ERR << "__CInnerCmdService::Loop() Read Error!";
                break;
            }
        }
        if (byteread == 0)
        {
            // no data to read
            PUMP_CORE_Sleep(500);
            continue;
        }
        //PUMP_CORE_INFO << "__CInnerCmdService::Loop() Read : " << outbuff;
        if (m_pfnReadCB)
        {
            m_pfnReadCB(outbuff, byteread, m_pOuterPip);
        }
    }
    PUMP_CORE_INFO << "Pipe Stoped!";
}

void CCmderServer::__CInnerCmdService::IsLoop()
{
    DWORD dwRet = ::WaitForSingleObject(m_struProcInfo.hProcess, INFINITE);
    while (dwRet == WAIT_TIMEOUT)
    {
        dwRet = ::WaitForSingleObject(m_struProcInfo.hProcess, INFINITE);
    }
    DWORD lpExitCode;
    ::GetExitCodeProcess(m_struProcInfo.hProcess, &lpExitCode);

    if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
    {
        PUMP_CORE_INFO<< "[END] Pipe Stoped!";
    }
}

CCmderServer::CCmderServer()
    : m_bOpen(PUMP_FALSE)
    , m_bStopRecvThx(PUMP_TRUE)
{
    
}

CCmderServer::~CCmderServer()
{
    this->Close();
    m_bOpen = PUMP_FALSE;
}

DWORD CALLBACK  CCmderServer::OnWirteCB(const char* pBuf, DWORD dwSize, void* data)
{
    CCmderServer * pthis = (CCmderServer *)data;
    //PUMP_CORE_INFO << "CCmderServer Send: " << pBuf;
    pthis->m_pHPipeOut->Write(pBuf, dwSize, NULL);
    return PUMP_OK;
}

pump_int32_t CCmderServer::Open()
{
    //::Pump::Core::CUUID objUuid;
    //std::string strPipeName = kCmdNamePipeIn;
    //strPipeName += objUuid.GetUUID();
    std::string strPipeName, strUUID = __CPumpCoreGlobalCtrl::GetCmdSessionMgr() ? __CPumpCoreGlobalCtrl::GetCmdSessionMgr()->CmdServerGetUUID() : "";
    if (strUUID.empty())
    {
        return PUMP_ERROR;
    }
    strPipeName = kCmdNamePipeIn + strUUID;
    m_pHPipeOut = CPipeHandle::CreateNamedPipeServer(
        strPipeName.c_str()
        , CPipeHandle::PUMP_PIPE_DEPLEX
        , PUMP_FALSE);
    if (!m_pHPipeOut)
    {
        PUMP_CORE_ERR << strPipeName.c_str() << " Create Failed";
        return PUMP_ERROR;
    }
    m_pHPipeOut->ConnectNamedPipeServer();

    strPipeName = kCmdNamePipeOut + strUUID;
    m_pHPipeIn = CPipeHandle::CreateNamedPipeServer(
        strPipeName.c_str()
        , CPipeHandle::PUMP_PIPE_DEPLEX
        , PUMP_FALSE);
    if (!m_pHPipeIn)
    {
        PUMP_CORE_ERR << strPipeName.c_str() << " Create Failed" << PUMP_CORE_GetSystemError();
        CPipeHandle::DestroyPipe(m_pHPipeOut);
        return PUMP_ERROR;
    }
    m_pHPipeIn->ConnectNamedPipeServer();

    if (!__CPumpCoreGlobalCtrl::GetCmdSessionMgr())
    {
        PUMP_CORE_ERR << "__CPumpCoreGlobalCtrl::GetCmdSessionMgr() is NULL";
        CPipeHandle::DestroyPipe(m_pHPipeIn);
        CPipeHandle::DestroyPipe(m_pHPipeOut);
        return PUMP_ERROR;
    }

    m_hThx = ::CreateThread(NULL, 0, __ReadCB, this, 0, &m_dwThxID);
    if (INVALID_HANDLE_VALUE == m_hThx)
    {
        Close();
    }
    m_bStopRecvThx = PUMP_FALSE;
    m_innerCmdService.SetReadCB(CCmderServer::OnWirteCB, this);
    if (m_innerCmdService.Open()==PUMP_ERROR)
    {
        return PUMP_ERROR;
    }
    return 0;
}

pump_int32_t CCmderServer::Close()
{
    m_bStopRecvThx = PUMP_TRUE;
    ::WaitForSingleObject(m_hThx, INFINITE); // must close thread first then close pipe, otherwise, may cause crash.
    ::Pump::Core::CPipeHandle::DestroyPipe(m_pHPipeIn); ///< server <- client
    ::Pump::Core::CPipeHandle::DestroyPipe(m_pHPipeOut); ///< server -> client
    return PUMP_OK;
}

DWORD __stdcall  CCmderServer::__ReadCB(void *pData)
{
    CCmderServer * pthis = (CCmderServer*)pData;
    char szBuf[1024] = { 0 };
    pump_uint32_t ReadNum = 0;
    while (!pthis->m_bStopRecvThx)
    {
        ReadNum = 0;
        memset(szBuf, 0, sizeof(szBuf));
        
        if (pthis->m_pHPipeIn->Read(szBuf, sizeof(szBuf) - 1, &ReadNum)  == PUMP_ERROR)
        {
            pthis->m_pHPipeIn->Close();
            PUMP_CORE_ERR<<"CCmderServer::__ReadCB() fail to read data from pipe";
            break;
        }
        else {
            if (ReadNum==0)
            {
                // 无数据可读
                PUMP_CORE_Sleep(500);
                continue;
            }
            //处理接收到的数据
            pthis->__ProcRecvedData(szBuf, ReadNum);
        }
    }
    PUMP_CORE_ERR << "CCmderServer::__ReadCB() finished";
    return 0;
}

void CCmderServer::__ProcRecvedData(const char* szBuf, DWORD dwSize)
{
    //m_strCmdCatch += szBuf;
    PUMP_CORE_INFO << "CCmderServer Recv: " << szBuf;
    if (!m_innerCmdService.InputCommand(szBuf))
    {
        
    }
}
#endif // PUMP_OS_WINDOWS

}
}
}

