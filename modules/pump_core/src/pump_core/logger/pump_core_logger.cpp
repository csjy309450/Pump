/**
 * @file pump_core_logger.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of log control interface.
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

#include "pump_core/logger/__pump_core_inner_logger.h"
#include "pump_core/logger/pump_core_logger.h"
#include "glog/logging.h"

namespace Pump
{
namespace Core
{

class CLoggerManagment
    : public CNonCopyable
{
public:
    static int CreateLoggerManagment(LPPUMP_CORE_LOG_CONF pConf)
    {
        if (sm_pLogMgr != NULL && sm_bInit)
        {
            // TODO error: reinit
            return -1;
        }
        sm_pLogMgr = new(std::nothrow) CLoggerManagment();
        if (sm_pLogMgr == NULL)
        {
            // TODO error: failed init
            return -1;
        }
        sm_pLogMgr->Init(pConf);
        return 0;
    }

    static bool IsLoggerInit()
    {
        return sm_bInit;
    }

private:
    CLoggerManagment() {}

    ~CLoggerManagment() {}

    static void Init(LPPUMP_CORE_LOG_CONF pConf)
    {
        if (pConf)
        {
            google::InitGoogleLogging("debug");
            switch (pConf->emLogLevel)
            {
            case PUMP_LOG_INFO:
                google::SetStderrLogging(google::GLOG_INFO);
                if (pConf->szFilePath)
                {
                    google::SetLogDestination(google::GLOG_INFO, pConf->szFilePath);
                }
                break;
            case PUMP_LOG_WARNING:
                google::SetStderrLogging(google::GLOG_WARNING);
                if (pConf->szFilePath)
                {
                    google::SetLogDestination(google::GLOG_WARNING, pConf->szFilePath);
                }
                break;
            case PUMP_LOG_ERROR:
                google::SetStderrLogging(google::GLOG_ERROR);
                if (pConf->szFilePath)
                {
                    google::SetLogDestination(google::GLOG_ERROR, pConf->szFilePath);
                }
                break;
            default:
                google::SetStderrLogging(google::GLOG_ERROR);
                if (pConf->szFilePath)
                {
                    google::SetLogDestination(google::GLOG_ERROR, pConf->szFilePath);
                }
                break;
            }
        }
        else
        {
            google::InitGoogleLogging("anonym");
            google::SetStderrLogging(google::GLOG_ERROR);
        }
        sm_bInit = true;
    }

private:
    static CLoggerManagment *sm_pLogMgr;
    static bool sm_bInit;
};

CLoggerManagment *CLoggerManagment::sm_pLogMgr = NULL;
bool CLoggerManagment::sm_bInit = false;

class CGLogRecorder
    : public CLogRecorderBase
{
#define __GLOG_OUTPUT(val) \
if(m_pLogMessage==NULL) \
  return this; \
static_cast<google::LogMessage*>(m_pLogMessage)->stream()<<val; \
return this;

public:
    CGLogRecorder()
        : m_pLogMessage(NULL) 
    {
        google::InitGoogleLogging("debug");
    }

    virtual ~CGLogRecorder()
    {
        google::ShutdownGoogleLogging();
        if (m_pLogMessage)
            delete static_cast<google::LogMessage *>(m_pLogMessage);
    }

public:
    virtual pump_int32_t Set(const PUMP_CORE_LOG_CONF & struConf)
    {
        CLogRecorderBase::Set(struConf);
        switch (m_struConf.emLogLevel)
        {
        case PUMP_LOG_INFO:
            google::SetStderrLogging(google::GLOG_INFO);
            if (m_struConf.szFilePath)
            {
                google::SetLogDestination(google::GLOG_INFO, m_struConf.szFilePath);
            }
            break;
        case PUMP_LOG_WARNING:
            google::SetStderrLogging(google::GLOG_WARNING);
            if (m_struConf.szFilePath)
            {
                google::SetLogDestination(google::GLOG_WARNING, m_struConf.szFilePath);
            }
            break;
        case PUMP_LOG_ERROR:
            google::SetStderrLogging(google::GLOG_ERROR);
            if (m_struConf.szFilePath)
            {
                google::SetLogDestination(google::GLOG_ERROR, m_struConf.szFilePath);
            }
            break;
        default:
            google::SetStderrLogging(google::GLOG_ERROR);
            if (m_struConf.szFilePath)
            {
                google::SetLogDestination(google::GLOG_ERROR, m_struConf.szFilePath);
            }
            break;
        }
        return PUMP_OK;
    }
    virtual CLogRecorderBase* Begin(const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel)
    {
        if (m_pLogMessage)
        {
            delete static_cast<google::LogMessage *>(m_pLogMessage);
            m_pLogMessage = NULL;
        }
        if (emLogLevel == PUMP_LOG_INFO)
        {
            m_pLogMessage = new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_INFO);
        }
        else if (emLogLevel == PUMP_LOG_WARNING)
        {
            m_pLogMessage = new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_WARNING);
        }
        else if (emLogLevel == PUMP_LOG_ERROR)
        {
            m_pLogMessage = new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_ERROR);
        }
        else
        {
            m_pLogMessage = new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_INFO);
        }
       
        return this;
    }
    virtual void End()
    {
        if (m_pLogMessage)
        {
            delete static_cast<google::LogMessage *>(m_pLogMessage);
            m_pLogMessage = NULL;
        }
    }
    virtual CLogRecorderBase*operator<< (bool val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (short val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual  CLogRecorderBase*operator<< (char val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (unsigned short val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (int val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual   CLogRecorderBase*operator<< (unsigned int val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (long val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (unsigned long val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual  CLogRecorderBase*operator<< (float val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (double val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (long double val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (void* val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (const char* val)
    {
        __GLOG_OUTPUT(val);
    }
    virtual CLogRecorderBase*operator<< (const std::string & val)
    {
        __GLOG_OUTPUT(val);
    }
private:
    google::LogMessage * m_pLogMessage;
};

CLogRecorderMgr::CLogRecorderMgr()
{

}
CLogRecorderMgr::~CLogRecorderMgr()
{

}
CLogRecorderBase * CLogRecorderMgr::Create(PUMP_LOG_RECORED_TYPE emType)
{
    switch (emType)
    {
    case PUMP_LOG_RECORED_GLOG:
    default:
        return new (std::nothrow) CGLogRecorder();
    }
    return PUMP_NULL;
}
pump_int32_t CLogRecorderMgr::Destroy(CLogRecorderBase * pLogRecorder)
{
    if (pLogRecorder)
    {
        delete pLogRecorder;
    }
    return PUMP_OK;
}

CLogRecorderKeeper::CLogRecorderKeeper(CLogRecorderBase* pLogRecorder)
    : ::Pump::Core::CGlobalResouceKeeper<CLogRecorderBase>(pLogRecorder)
{
    if (!::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl || !pLogRecorder)
    {
        return;
    }
    ::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl->m_csLogRecorder.readLock();
}

CLogRecorderKeeper::CLogRecorderKeeper(CLogRecorderKeeper & other)
    : ::Pump::Core::CGlobalResouceKeeper<CLogRecorderBase>(other)
{
    if (!::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl || !other.GetPtr())
    {
        return;
    }
    ::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl->m_csLogRecorder.readLock();
}

CLogRecorderKeeper::~CLogRecorderKeeper()
{
    if (!::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl || !this->GetPtr())
    {
        return;
    }
    ::Pump::Core::CGlobalCtrlBase::s_pGlobalCtrl->m_csLogRecorder.readUnlock();
}

CLogGuide::CLogGuide(CLogRecorderKeeper & reflogRecorder, const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel)
    : m_refLogRecorder(reflogRecorder)
{
    if (m_refLogRecorder.GetPtr())
    {
        m_refLogRecorder.GetPtr()->Begin(szFile, nLine, emLogLevel);
    }
}

CLogGuide::~CLogGuide()
{
    if (m_refLogRecorder.GetPtr())
    {
        m_refLogRecorder.GetPtr()->End();
    }
}

CLogGuide& CLogGuide::operator<< (bool val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (short val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (char val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (unsigned short val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (int val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (unsigned int val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (long val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (unsigned long val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (float val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (double val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (long double val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (void* val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (const char* val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}
CLogGuide& CLogGuide::operator<< (const std::string & val)
{
    if (m_refLogRecorder.GetPtr())
    {
        (*m_refLogRecorder.GetPtr()) << val;
    }
    return *this;
}

#define ISLOGGERINIT(x) if(CLoggerManagment::IsLoggerInit()==false) return x;

#define PUMP_CORE_LOG_OUTPUT(val) \
ISLOGGERINIT(*this); \
if(m_pLogMessage==NULL) \
  return *this; \
static_cast<google::LogMessage*>(m_pLogMessage)->stream()<<val; \
return *this;

CLogger::CLogger(const char *szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel)
    : m_pLogMessage((!CLoggerManagment::IsLoggerInit()) ?
NULL :
     ((emLogLevel == PUMP_LOG_INFO) ?
     new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_INFO) :
     ((emLogLevel == PUMP_LOG_WARNING) ?
     new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_WARNING) :
     ((emLogLevel == PUMP_LOG_ERROR) ?
     new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_ERROR) :
     new(std::nothrow) google::LogMessage(szFile, nLine, google::GLOG_INFO))))) {}

CLogger::~CLogger()
{
    if (m_pLogMessage)
        delete static_cast<google::LogMessage *>(m_pLogMessage);
}

CLogger &CLogger::operator<<(bool val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(short val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(char val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(unsigned short val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(int val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(unsigned int val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(long val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(unsigned long val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(float val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(double val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(long double val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(void *val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(const char *val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

CLogger &CLogger::operator<<(const std::string &val)
{
    PUMP_CORE_LOG_OUTPUT(val);
}

}
}

PUMP_CORE_API void PUMP_CALLBACK  __PUMP_CORE_Test_new_logger()
{
    //PUMP_CORE_LOG_CONF struLogCong;
    //memset(&struLogCong, 0, sizeof(struLogCong));
    //struLogCong.szFilePath = "";
    //struLogCong.emLogLevel = PUMP_LOG_INFO;
    //::Pump::Core::__CPumpCoreGlobalCtrl::SetLogger(struLogCong);
    //__PUMP_CORE_INFO << "-------test begin-------";
}

/**
 * pump core log init API.  
 */
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_InjectLocalLogger(pump_handle_t hLogger)
{
    //::Pump::Core::CLoggerManagment::CreateLoggerManagment(pConf);
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    return ::Pump::Core::__CPumpCoreGlobalCtrl::SetLogger(hLogger);
}

PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_LoggerCreate(PUMP_LOG_RECORED_TYPE emType)
{
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_NULL;
    }
    return ::Pump::Core::__CPumpCoreGlobalCtrl::GetLoggerMgr()
        ? ::Pump::Core::__CPumpCoreGlobalCtrl::GetLoggerMgr()->Create(emType)
        : PUMP_NULL;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerDestroy(pump_handle_t hLog)
{
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    if (PUMP_INVALID_HANDLE == hLog || PUMP_NULL == hLog)
    {
        return PUMP_ERROR;
    }
    return ::Pump::Core::__CPumpCoreGlobalCtrl::GetLoggerMgr() ?
        ::Pump::Core::__CPumpCoreGlobalCtrl::GetLoggerMgr()->Destroy((::Pump::Core::CLogRecorderBase *)hLog)
        : PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerConfig(pump_handle_t hLog, const LPPUMP_CORE_LOG_CONF pConf)
{
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    if (hLog==PUMP_NULL || !pConf)
    {
        // error: param error.
        return PUMP_ERROR;
    }
    return ((::Pump::Core::CLogRecorderBase *)hLog)->Set(*pConf);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerWrite(pump_handle_t hLog, const char* szFormat, ...)
{
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    if (hLog == PUMP_NULL || !szFormat)
    {
        // error: param error.
        return PUMP_ERROR;
    }
    return PUMP_OK;
}
