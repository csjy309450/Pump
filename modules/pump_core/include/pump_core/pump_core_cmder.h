/**
 * @file pump_core_cmder.h
 * @brief Library Header.
 * @details Declaration of CMD control interface.
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

#ifndef PUMP_CORE_CMDER_H
#define PUMP_CORE_CMDER_H
#include "pump_macro/pump_pre.h"
#include <list>
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_pipe.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/pump_core_statemachine.h"
#include "pump_core/pump_core_global_resource_mgr.h"
#include "pump_core/pump_core_uuid.h"

namespace Pump
{
namespace Core
{
namespace Cmder
{

class CCmdSessionMgr : public ::Pump::Core::CGlobalResourceMgr
{
public:
    //void InsertCmdServer(const std::string & strCmdServerName)
    //{
    //    ::Pump::Core::Thread::CMutexGuilder locker(m_csCmdServerCatch);
    //    m_listCmdServerCatch.push_back(strCmdServerName);
    //}
    //std::string PopCmdServer()
    //{
    //    ::Pump::Core::Thread::CMutexGuilder locker(m_csCmdServerCatch);
    //    if (m_listCmdServerCatch.empty())
    //    {
    //        return "";
    //    }
    //    std::string strName = m_listCmdServerCatch.front();
    //    m_listCmdServerCatch.pop_front();
    //    return strName;
    //}
    std::string CmdServerGetUUID()
    {
        if (m_strCmdUUID.empty())
        {
            ::Pump::Core::CUUID uuid('_');
            m_strCmdUUID = uuid.GetUUID();
        }
        return m_strCmdUUID;
    }
    std::string CmdClientGetUUID()
    {
        std::string strOut = m_strCmdUUID;
        m_strCmdUUID.clear();
        return strOut;
    }
private:
    ::Pump::Core::Thread::CMutex m_csCmdServerCatch;
    std::string m_strCmdUUID;
};

/* TODO 20200626 Need add "ctrl+c" cmd than interrupting running process.
*/
class PUMP_CORE_CLASS CCmderClient
{
public:
    friend class CThxRead;
    typedef enum tagPUMP_CMDER_CLIENT_CB_TYPE
    {
        PUMP_CMDER_CLIENT_CB_DEFAULT = 0,
        PUMP_CMDER_CLIENT_CB_OPEN,
        PUMP_CMDER_CLIENT_CB_RECV,
        PUMP_CMDER_CLIENT_CB_BEGIN,
        PUMP_CMDER_CLIENT_CB_END,
        PUMP_CMDER_CLIENT_CB_CLOSE
    } PUMP_CMDER_CLIENT_CB_TYPE;
private:
    class CThxRead
        : public ::Pump::Core::Thread::CThread
    {
    public:
        CThxRead(pump_pvoid_t pdata);
    private:
        virtual pump_void_t * ThreadCallback(pump_void_t * pData);
    private:
    };
    typedef ::Pump::Core::CStateMachineBase CCmderState;
public:
    CCmderClient();
    ~CCmderClient();
    pump_int32_t Open();
    pump_int32_t Close();
    pump_int32_t Write(const char * szBuff, pump_uint32_t dwSize);
private:
    /*
     * @brief User data callback, feedback CMD result to user.
     */
    virtual pump_void_t ReadCallback(pump_uint32_t iCBType, const char * szBuff, pump_uint32_t dwSize, pump_pvoid_t pData);
private:
    pump_bool_t m_bOpen;
    pump_bool_t m_bAvailable;
    ::Pump::Core::CPipeHandle * m_pHPipeIn; ///< server -> client
    ::Pump::Core::CPipeHandle * m_pHPipeOut; ///< server <- client
    CCmderState m_state;
    CThxRead m_thxRead;
    std::string m_strLastCmd;
};

class PUMP_CORE_CLASS CCmderServer
{
public:
    typedef DWORD(CALLBACK *fnOnReadCB)(const char* pBuf, DWORD dwSize, void* data);

    /*
     * @brief Inner service in CMD server. Use for start CMD process, then control anonymous-pipe to communication with it.
     */
    class PUMP_CORE_CLASS __CInnerCmdService
    {
    public:
        __CInnerCmdService();
        ~__CInnerCmdService();
        pump_int32_t Open();
        pump_int32_t Close();
        void Loop();
        void IsLoop();
        pump_int32_t InputCommand(const char *);
        void SetReadCB(fnOnReadCB pfn, CCmderServer * p);
    private:
        // FIXME 20200504 Replace CThread with __ThxReadCB() and __ThxWaitExitCB() callback.
        static DWORD __stdcall __ThxReadCB(void *pData);
        static DWORD __stdcall __ThxWaitExitCB(void *pData);
    private:
        ::Pump::Core::CPipeHandle * m_pHPipeIn; ///< __CInnerCmdService <- cmd.exe
        ::Pump::Core::CPipeHandle * m_pHPipeOut; ///< __CInnerCmdService -> cmd.exe
        HANDLE m_hProcHandle;
        HANDLE m_hThxWaitExit; 
        HANDLE m_hThxRead; ///< inner thread 
        pump_bool_t m_bStopThxRead;
        PROCESS_INFORMATION m_struProcInfo;
        STARTUPINFOA m_struStartupInfo;
        fnOnReadCB m_pfnReadCB;
        CCmderServer * m_pOuterPip;
    };

    // FIXME 20200504 Replace CThxRead with __ReadCB()
    //class CThxRead
    //    : public ::Pump::Core::Thread::CThread
    //{
    //public:
    //    CThxRead(pump_pvoid_t pdata);
    //private:
    //    virtual pump_void_t * ThreadCallback(pump_void_t * pData);
    //private:
    //};
public:
    static DWORD CALLBACK  OnWirteCB(const char* pBuf, DWORD dwSize, void* data);
    CCmderServer();
    ~CCmderServer();
    pump_int32_t Open();
    pump_int32_t Close();
private:
    static DWORD __stdcall  __ReadCB(void *pData);
    void __ProcRecvedData(const char* szBuf, DWORD dwSize);
private:
    pump_bool_t m_bOpen;
    pump_bool_t m_bStopRecvThx;
    ::Pump::Core::CPipeHandle * m_pHPipeIn; ///< server <- client
    ::Pump::Core::CPipeHandle * m_pHPipeOut; ///< server -> client
    HANDLE m_hThx;
    DWORD m_dwThxID;
    __CInnerCmdService m_innerCmdService;
    //CThxRead m_thxRead;
};

}
}
}

#endif // PUMP_CORE_CMDER_H

