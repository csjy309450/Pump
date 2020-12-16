/**
 * @file pump_core_pipe.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of pipe control interface.
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

#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_pipe.h"

namespace Pump
{
namespace Core
{

CPipeHandle* CPipeHandle::CreatePipeHandle()
{
    CPipeHandle* pHPipe = new (std::nothrow)CPipeHandle();
    if (!pHPipe)
    {
        return PUMP_NULL;
    }
    return pHPipe;
}

CPipeHandle* CPipeHandle::CreateNamedPipeServer(
    const pump_char_t * szName,
    pump_uint32_t dwOpenMode,
    pump_bool_t bAsynic,
    pump_uint32_t dwReadBufSize,
    pump_uint32_t dwWriteBufSize)
{
    CPipeHandle* pHPipe = new (std::nothrow)CPipeHandle();
    if (!pHPipe)
    {
        return PUMP_NULL;
    }
    if (pHPipe->OpenNamedPipeServer(szName, dwOpenMode, bAsynic, dwReadBufSize, dwWriteBufSize) == PUMP_ERROR)
    {
        delete pHPipe;
        pHPipe = PUMP_NULL;
        return PUMP_NULL;
    }
    return pHPipe;
}

CPipeHandle* CPipeHandle::CreateNamedPipeClient(
    const pump_char_t * szName,
    pump_uint32_t dwOpenMode)
{
    CPipeHandle* pHPipe = new (std::nothrow)CPipeHandle();
    if (!pHPipe)
    {
        return PUMP_NULL;
    }
    if (pHPipe->OpenNamedPipeClient(szName, dwOpenMode) == PUMP_ERROR)
    {
        delete pHPipe;
        pHPipe = PUMP_NULL;
        return PUMP_NULL;
    }
    return pHPipe;
}

CPipeHandle* CPipeHandle::CreateAnonyPipe()
{
    CPipeHandle* pHPipe = new (std::nothrow)CPipeHandle();
    if (!pHPipe)
    {
        return PUMP_NULL;
    }
    if (pHPipe->OpenAnonyPipe() == PUMP_ERROR)
    {
        delete pHPipe;
        pHPipe = PUMP_NULL;
        return PUMP_NULL;
    }
    return pHPipe;
}

pump_int32_t CPipeHandle::DestroyPipe(CPipeHandle* pHPipe)
{
    delete pHPipe;
    return PUMP_OK;
}

CPipeHandle::CPipeHandle()
    : ::Pump::Core::CHandleBase(PUMP_HANDLE_PIPE)
    , m_bOpen(PUMP_FALSE)
    , m_dwOpenMode(PUMP_PIPE_DEFAULT)
    , m_bAsynic(PUMP_FALSE)
{
    ::memset(&m_struOverlapped, 0, sizeof(m_struOverlapped));
    ::memset(m_szPipeName, 0, sizeof(m_szPipeName));
    PUMP_RAW_PIPE * pHandle = new (std::nothrow)PUMP_RAW_PIPE();
    if (pHandle == PUMP_NULL)
    {
        return;
    }
    ::memset(pHandle, -1, sizeof(pHandle));
    m_handle = pHandle;
}


CPipeHandle::~CPipeHandle()
{
    if (m_bOpen)
    {
        PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
        if (m_szPipeName[0] == 0)
        {
            ::CloseHandle(pHandle->arrAnonyPipe_[0]);
            ::CloseHandle(pHandle->arrAnonyPipe_[1]);
        }
        else
        {            
            ::CloseHandle(pHandle->hNamedPipe_);
        }
    }
    delete m_handle;
    m_handle = PUMP_NULL;
}

pump_int32_t CPipeHandle::OpenAnonyPipe()
{
#ifdef PUMP_OS_WINDOWS
    m_bAsynic = PUMP_FALSE;
    SECURITY_ATTRIBUTES struSA = { 0 };
    HANDLE arrPipe[2] = { 0 };
    struSA.nLength = sizeof(SECURITY_ATTRIBUTES);
    struSA.bInheritHandle = true;
    struSA.lpSecurityDescriptor = NULL;
    if (!::CreatePipe(&arrPipe[0], &arrPipe[1], &struSA, 0))  // create read in pipe
    {
        PUMP_CORE_ERR("[0x01]Read 流创建失效");
        return PUMP_ERROR;
    }
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
    ::memcpy(pHandle->arrAnonyPipe_, arrPipe, sizeof(arrPipe));
#elif (defined PUMP_OS_POSIX)
    m_bAsynic = PUMP_FALSE;
    pump_fd_t arrPipe[2] = { 0 };
    int ret = ::pipe(arrPipe);
    if (ret == -1)
    {
        PUMP_CORE_ERR << "pipe error " << errno << ", " << strerror(errno);
        return PUMP_ERROR;
    }
    __RWA_PIPE * pHandle = static_cast<__RWA_PIPE *>(m_handle);
    ::memcpy(pHandle->arrAnonyPipe_, arrPipe, sizeof(arrPipe));
#endif // PUMP_OS_WINDOWS
    m_bOpen = PUMP_TRUE;
    return PUMP_OK;
}

pump_int32_t CPipeHandle::OpenNamedPipeServer(
    const pump_char_t * szName
    , pump_uint32_t dwOpenMode
    , pump_bool_t bAsynic
    , pump_uint32_t dwReadBufSize
    , pump_uint32_t dwWriteBufSize)
{
#ifdef PUMP_OS_WINDOWS
    ::strncpy(m_szPipeName, szName, 128 - 1);
    m_bAsynic = bAsynic;
    m_dwOpenMode = 0;
    if (m_bAsynic)
    {
        m_dwOpenMode |= FILE_FLAG_OVERLAPPED;
    }
    if ((dwOpenMode == PUMP_PIPE_READ))
    {
        m_dwOpenMode |= PIPE_ACCESS_INBOUND;
    }
    else if ((dwOpenMode == PUMP_PIPE_WRITE))
    {
        m_dwOpenMode |= PIPE_ACCESS_OUTBOUND;
    }
    else if ((dwOpenMode == PUMP_PIPE_DEPLEX))
    {
        m_dwOpenMode |= PIPE_ACCESS_DUPLEX;
    }
    HANDLE hPipe = ::CreateNamedPipeA(
        szName,
        m_dwOpenMode,/*PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,*/
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS,
        PIPE_UNLIMITED_INSTANCES,
        dwWriteBufSize, dwReadBufSize,
        NMPWAIT_USE_DEFAULT_WAIT, NULL);
    if (INVALID_HANDLE_VALUE == hPipe)
    {
        PUMP_CORE_ERR ( "server failed to create named pipe");
        return PUMP_ERROR;
    }
    PUMP_CORE_INFO("server succed to create named pipe");
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
    pHandle->hNamedPipe_ = hPipe;
#elif (defined PUMP_OS_POSIX)
    ::strncpy(m_szPipeName, szName, 128-1);
    m_bAsynic = bAsynic;
    if (dwOpenMode | PUMP_PIPE_READ != 0)
    {
        m_dwOpenMode |= O_RDONLY;
    }
    else if (dwOpenMode | PUMP_PIPE_WRITE != 0)
    {
        m_dwOpenMode |= O_WRONLY;
    }
    else if (dwOpenMode | PUMP_PIPE_DEPLEX != 0)
    {
        m_dwOpenMode |= O_RDWR;
    }
    pump_fd_t hPipe = ::open(m_szPipeName, m_dwOpenMode);
    if (hPipe < 0)
    {
        return PUMP_ERROR;
    }
    ::umask(0);
    int ret = ::mkfifo(hPipe, 0664);
    if (ret < 0)
    {
        ::close(hPipe);
        return PUMP_ERROR;
    }
    if (m_bAsynic)
    {
        pump_int32_t iFlags = ::fcntl((pump_sock_t)m_handle, F_GETFL, 0);
        ret = ::fcntl(hPipe, F_SETFL, iFlags | O_NONBLOCK);
        if (ret < 0)
        {
            ::close(hPipe);
            return PUMP_ERROR;
        }
    }
    __RWA_PIPE * pHandle = static_cast<__RWA_PIPE *>(m_handle);
    pHandle->hNamedPipe_ = hPipe;
#endif // PUMP_OS_WINDOWS
    m_bOpen = PUMP_TRUE;
    return PUMP_OK;
}

pump_int32_t CPipeHandle::ConnectNamedPipeServer()
{
#ifdef PUMP_OS_WINDOWS
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
    HANDLE hPipe = pHandle->hNamedPipe_;
    BOOL ret = FALSE;
    if (m_bAsynic)
    {
        ret = ::ConnectNamedPipe(hPipe, &m_struOverlapped);
    }
    else
    {
        ret = ::ConnectNamedPipe(hPipe, NULL);
    }
    if (!ret)
    {
        DWORD dwError = ::GetLastError();
        switch (dwError)
        {
        case ERROR_IO_PENDING:
        case ERROR_PIPE_CONNECTED: // blocked operation is running.
            break;
        default:
        {
            PUMP_CORE_ERR("ConnectNamedPipe failed, err = %d",dwError);
            return PUMP_ERROR;
        }
        }
    }
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

pump_int32_t CPipeHandle::OpenNamedPipeClient(
    const pump_char_t * szName,
    pump_uint32_t dwOpenMode)
{
#ifdef PUMP_OS_WINDOWS
    if (!::WaitNamedPipeA(szName, 20000))
    {
        PUMP_CORE_ERR("Client Wait pipe failed: 20 second timed out. %d" , PUMP_CORE_GetSystemError());
        return PUMP_ERROR;
    }
    if ((dwOpenMode == PUMP_PIPE_READ))
    {
        m_dwOpenMode |= GENERIC_READ;
    }
    else if ((dwOpenMode == PUMP_PIPE_WRITE))
    {
        m_dwOpenMode |= GENERIC_WRITE;
    }
    else if ((dwOpenMode == PUMP_PIPE_DEPLEX))
    {
        m_dwOpenMode |= GENERIC_READ;
        m_dwOpenMode |= GENERIC_WRITE;
    }
    if (strlen(szName) >= sizeof(m_szPipeName))
    {
        PUMP_CORE_ERR("pipe name cannot exceed %d" , sizeof(m_szPipeName));
        return PUMP_ERROR;
    }
    strncpy(m_szPipeName, szName, sizeof(m_szPipeName));
    HANDLE hPipe = ::CreateFileA(
        szName,   // pipe name 
        m_dwOpenMode,
        0,              // no sharing 
        NULL,           // default security attributes
        OPEN_EXISTING,  // opens existing pipe 
        NULL,              // default attributes 
        NULL);          // no template file 
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        PUMP_CORE_ERR("Could not open pipe. Sys Err=%d" , ::GetLastError());
        return PUMP_ERROR;
    }
    PUMP_CORE_INFO("client connect pipe succed");
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
    pHandle->hNamedPipe_ = hPipe;
    //// Exit if an error other than ERROR_PIPE_BUSY occurs. 
    //DWORD err = ::GetLastError();
    //if (err != ERROR_PIPE_BUSY)
    //{
    //    PUMP_CORE_ERR << "pipe is busy. Sys Err=" << ::GetLastError();
    //    return PUMP_ERROR;
    //}
#elif (defined PUMP_OS_POSIX)
#endif // PUMP_OS_WINDOWS
    m_bOpen = PUMP_TRUE;
    return PUMP_OK;
}

pump_int32_t CPipeHandle::Close()
{
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
#ifdef PUMP_OS_WINDOWS
    if (m_szPipeName[0] == 0)
    { //匿名
        ::CloseHandle(pHandle->arrAnonyPipe_[0]);
        ::CloseHandle(pHandle->arrAnonyPipe_[1]);
    }
    else
    {
        ::CloseHandle(pHandle->hNamedPipe_);
    }
#elif (defined PUMP_OS_POSIX)
    if (m_szPipeName[0] == 0)
    { //anonymous pipe
        ::close(pHandle->arrAnonyPipe_[0]);
        ::close(pHandle->arrAnonyPipe_[1]);
    }
    else
    {
        ::close(pHandle->hNamedPipe_);
    }
#endif // PUMP_OS_WINDOWS
    memset(pHandle, -1, sizeof(pHandle));
    m_bOpen = PUMP_FALSE;
    return PUMP_OK;
}

pump_int32_t CPipeHandle::Write(const pump_void_t * szBuff, pump_uint32_t dwSize, pump_uint32_t* pNWrite)
{
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
#ifdef PUMP_OS_WINDOWS
    DWORD dwNWrite = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    if (m_szPipeName[0] == 0)
    { //anonymous pipe
        hPipe = pHandle->arrAnonyPipe_[1];
    }
    else
    {
        hPipe = pHandle->hNamedPipe_;
    }
    BOOL ret = ::WriteFile(
        hPipe
        , szBuff
        , dwSize
        , &dwNWrite
        , (m_bAsynic ? &m_struOverlapped : NULL));
    if (!ret)
    {
    }
    if (pNWrite)
    {
        *pNWrite = dwNWrite;
    }
#elif (defined PUMP_OS_POSIX)
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

pump_int32_t CPipeHandle::Read(pump_void_t * szBuff, pump_uint32_t dwSize, pump_uint32_t* pNRead)
{
    PUMP_RAW_PIPE * pHandle = static_cast<PUMP_RAW_PIPE *>(m_handle);
#ifdef PUMP_OS_WINDOWS
    DWORD dwNRead = 0, dwTotalBytesAvail = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    if (m_szPipeName[0] == 0)
    { //anonymous pipe
        hPipe = pHandle->arrAnonyPipe_[0];
    }
    else
    {
        hPipe = pHandle->hNamedPipe_;
    }
    /**
     * @note [yangzheng] ReadFile() is a blocked operation in synchronized pipe, even CloseHandle() can't release block, causes the program to fail to exit.
     * So should call PeekNamedPipe() before ReadFile() to check pipe data is readable, avoid reading operation block in ReadFile().
     */
    BOOL ret = ::PeekNamedPipe(hPipe, NULL, 0, &dwNRead, &dwTotalBytesAvail, 0);
    if (ret == FALSE || dwTotalBytesAvail <= 0)
    {
        // there are no data to read.
        if (pNRead)
        {
            *pNRead = 0;
        }
        return PUMP_OK;
    }
    ret = ::ReadFile(
        hPipe
        , szBuff
        , dwSize - 1
        , &dwNRead
        , (m_bAsynic ? &m_struOverlapped : NULL));
    if (!ret)
    {
        PUMP_CORE_ERR("CPipeHandle::Read() failed %d", PUMP_CORE_GetSystemError());
        if (pNRead)
        {
            *pNRead = dwNRead;
        }
        return PUMP_ERROR;
    }
    if (pNRead)
    {
        *pNRead = dwNRead;
    }
#elif (defined PUMP_OS_POSIX)
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

pump_bool_t CPipeHandle::Available() const
{
    return m_bOpen;
}

#ifdef PUMP_OS_WINDOWS
LPOVERLAPPED CPipeHandle::GetOverLapped()
{
    return &m_struOverlapped;
}
#endif // PUMP_OS_WINDOWS

}
}
