/**
 * @file pump_core_pipe.h
 * @brief Library Header.
 * @details Declaration of pipe control interface.
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

#ifndef PUMP_CORE_PIPE_H
#define PUMP_CORE_PIPE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_handle.h"

namespace Pump
{
namespace Core
{

#define PUMP_PIPE_DEFAULT_BUFFER_SIZE 1024

class PUMP_CORE_CLASS CPipeHandle
    : public ::Pump::Core::CHandleBase
{
public:
    enum PIPE_OPEN_MODE
    {
        PUMP_PIPE_DEFAULT = 0,
        PUMP_PIPE_READ,
        PUMP_PIPE_WRITE,
        PUMP_PIPE_DEPLEX
    };
    typedef union tagPUMP_RAW_PIPE
    {
#if (defined PUMP_OS_WINDOWS)
        pump_handle_t arrAnonyPipe_[2];
        pump_handle_t hNamedPipe_;
#elif (defined PUMP_OS_POSIX)
        pump_fd_t arrAnonyPipe_[2];
        pump_fd_t hNamedPipe_;
#endif // (defined PUMP_OS_WINDOWS)
    } PUMP_RAW_PIPE, *LPPUMP_RAW_PIPE;
public:
    static CPipeHandle* CreatePipeHandle();
    static CPipeHandle* CreateNamedPipeServer(
        const pump_char_t * szName,
        pump_uint32_t dwOpenMode, 
        pump_bool_t bAsynic,
        pump_uint32_t dwReadBufSize = PUMP_PIPE_DEFAULT_BUFFER_SIZE,
        pump_uint32_t dwWriteBufSize = PUMP_PIPE_DEFAULT_BUFFER_SIZE);
    static CPipeHandle* CreateNamedPipeClient(
        const pump_char_t * szName,
        pump_uint32_t dwOpenMode);
    static CPipeHandle* CreateAnonyPipe();
    static pump_int32_t DestroyPipe(CPipeHandle* pHPipe);

    pump_int32_t OpenNamedPipeServer(
        const pump_char_t * szName
        , pump_uint32_t dwOpenMode
        , pump_bool_t bAsynic
        , pump_uint32_t dwReadBufSize
        , pump_uint32_t dwWriteBufSize);
    pump_int32_t ConnectNamedPipeServer();
    pump_int32_t OpenNamedPipeClient(
        const pump_char_t * szName,
        pump_uint32_t dwOpenMode);
    pump_int32_t OpenAnonyPipe();
    pump_int32_t Close();
    pump_int32_t Write(
        const pump_void_t * szBuff, 
        pump_uint32_t dwSize, 
        pump_uint32_t* pNWrite);
    pump_int32_t Read(
        pump_void_t * szBuff, 
        pump_uint32_t dwSize, 
        pump_uint32_t* pNRead);
    virtual pump_bool_t Available() const;
#ifdef PUMP_OS_WINDOWS
    LPOVERLAPPED GetOverLapped();
#endif // PUMP_OS_WINDOWS
private:
    CPipeHandle();
    virtual ~CPipeHandle();

protected:
#ifdef PUMP_OS_WINDOWS
    OVERLAPPED m_struOverlapped;
#endif // PUMP_OS_WINDOWS
    pump_bool_t m_bOpen;
    pump_char_t m_szPipeName[128];
    pump_uint32_t m_dwOpenMode;
    pump_bool_t m_bAsynic;
};

}
}

void* PUMP_CORE_CreateNamedPipe();
pump_int32_t PUMP_CORE_DestroyNamedPipe(void* pHPipe);

#endif // PUMP_CORE_PIPE_H

