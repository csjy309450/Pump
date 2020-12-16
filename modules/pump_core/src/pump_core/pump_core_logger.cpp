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

#include <ctime> 
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/__pump_core_inner_logger.h"
#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_file.h"
#include "pump_core/pump_core_string.h"
#ifdef PUMP_USING_GLOG
#include "glog/logging.h"
#endif // PUMP_USING_GLOG

namespace Pump
{
namespace Core
{

CLogData::CLogData() 
    : m_tm(0)
    , m_tid(0)
    , m_byLevel(0)
    , m_nSrcLine(0)
{
    memset(m_strModular, 0, sizeof(m_strModular));
    memset(m_strSrcPos, 0, sizeof(m_strSrcPos));
}

CLogData::~CLogData() 
{
}

CLogData::CLogData(const CLogData & other)
    : m_tm(other.m_tm)
    , m_tid(other.m_tid)
    , m_byLevel(other.m_byLevel)
    , m_nSrcLine(other.m_nSrcLine)
{
    strncpy(m_strSrcPos, other.m_strSrcPos, sizeof(m_strSrcPos));
    strncpy(m_strModular, other.m_strModular, sizeof(m_strModular));
}

CLogData & CLogData::operator=(const CLogData & other)
{
    m_tm = other.m_tm;
    m_tid = other.m_tid;
    m_byLevel = other.m_byLevel;
    m_nSrcLine = (other.m_nSrcLine);
    strncpy(m_strSrcPos, other.m_strSrcPos, sizeof(m_strSrcPos));
    strncpy(m_strModular, other.m_strModular, sizeof(m_strModular));
    return *this;
}

void CLogData::SetTm(long long val)
{
    m_tm = val;
}

void CLogData::ResetTm()
{
    m_tm = 0;
}

long long CLogData::GetTm()
{
    return m_tm;
}

void CLogData::SetTID(unsigned long long val)
{
    m_tid = val;
}

void CLogData::ResetTID()
{
    m_tid = 0;
}

unsigned  long long CLogData::GetTID()
{
    return m_tid;
}

void CLogData::SetLevel(unsigned char val)
{
    m_byLevel = val;
}

void CLogData::ResetLevel()
{
    m_byLevel = 0;
}

unsigned char CLogData::GetLevel()
{
    return m_byLevel;
}

void CLogData::SetModular(const char* val)
{
    strncpy(m_strModular, val, sizeof(m_strModular));
}

void CLogData::ResetModular()
{
    memset(m_strModular, 0, sizeof(m_strModular));
}

const char* CLogData::GetModular()
{
    return m_strModular;
}

void CLogData::SetSrcPos(const char* val)
{
    strncpy(m_strSrcPos, val, sizeof(m_strSrcPos));
}

void CLogData::ResetSrcPos()
{
    memset(m_strSrcPos, 0, sizeof(m_strSrcPos));
}

const char* CLogData::GetSrcPos()
{
    return m_strSrcPos;
}

void CLogData::SetSrcLine(pump_uint32_t val)
{
    m_nSrcLine = val;
}

void CLogData::ResetSrcLine()
{
    m_nSrcLine = 0;
}

pump_uint32_t CLogData::GetSrcLine()
{
    return m_nSrcLine;
}

void CLogData::SetMessage(
    PUMP_CORE_LOG_LEVEL emLevel,
    const char* szFile,
    unsigned int lLine,
    const char* szFormate,
    va_list args)
{
    m_strMsg = PUMP_CORE_Vprintf(szFormate, args);
}

void CLogData::ResetMessage()
{
    m_strMsg.clear();
}

const char * CLogData::GetMessage()
{
    return m_strMsg.c_str();
}

pump_uint32_t CLogData::GetMessageSize() const
{
    return (pump_uint32_t)m_strMsg.size();
}

void CLogData::__Time2Str(char * pBuf, size_t dwSize)
{
    if (pBuf == NULL || dwSize < 17)
    {
        return;
    }
    time_t t_time;
    struct tm* t_tm;
    ::time(&t_time);
    t_tm = ::localtime(&t_time);
    sprintf(pBuf, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d\0",
        t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday,
        t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
}

CLogRecorderBase::CLogRecorderBase()
    : m_bInit(PUMP_FALSE)
    , m_emType(PUMP_CORE_LOG_RECORED_DEFAULT)
{
    memset(&m_struConf, 0, sizeof(m_struConf));
}

CLogRecorderBase::CLogRecorderBase(PUMP_CORE_LOG_RECORED_TYPE emType)
    : m_bInit(PUMP_FALSE)
    , m_emType(emType)
{
    memset(&m_struConf, 0, sizeof(m_struConf));
}

CLogRecorderBase::~CLogRecorderBase()
{}

pump_int32_t CLogRecorderBase::SetConfig(const PUMP_CORE_LOG_CONF & refConf)
{
    m_csConf.Lock();
    m_struConf = refConf;
    m_csConf.Unlock();
    return PUMP_OK;
}

PUMP_CORE_LOG_CONF CLogRecorderBase::GetConfig() const
{
    return m_struConf;
}

#ifdef PUMP_USING_GLOG
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
        //: m_pLogMessage(NULL) 
    {
        google::InitGoogleLogging("debug");
    }

    virtual ~CGLogRecorder()
    {
        google::ShutdownGoogleLogging();
        //if (m_pLogMessage)
        //    delete static_cast<google::LogMessage *>(m_pLogMessage);
    }

public:
    virtual int Init(const PUMP_CORE_LOG_CONF & refConf) 
    {
        return 0;
    }
    virtual int Destroy() 
    {
        return 0;
    }
    virtual pump_int32_t SetConfig(const PUMP_CORE_LOG_CONF & struConf)
    {
        CLogRecorderBase::SetConfig(struConf);
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

    virtual int WriteLine(CLogData & data)
    {
        switch (data.GetLevel())
        {
        case PUMP_LOG_INFO:
            google::LogMessage(data.GetSrcPos(), data.GetSrcLine(), google::GLOG_INFO).stream() << data.GetMessage();
            break;
        case PUMP_LOG_WARNING:
            google::LogMessage(data.GetSrcPos(), data.GetSrcLine(), google::GLOG_WARNING).stream() << data.GetMessage();
            break;
        case PUMP_LOG_ERROR:
            google::LogMessage(data.GetSrcPos(), data.GetSrcLine(), google::GLOG_ERROR).stream() << data.GetMessage();
            break;
        }
        return 0;
    }

    /*virtual CLogRecorderBase* Begin(const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel)
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
    }*/
    //virtual CLogRecorderBase*operator<< (bool val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (short val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual  CLogRecorderBase*operator<< (char val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (unsigned short val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (int val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual   CLogRecorderBase*operator<< (unsigned int val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (long val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (unsigned long val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual  CLogRecorderBase*operator<< (float val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (double val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (long double val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (void* val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (const char* val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
    //virtual CLogRecorderBase*operator<< (const std::string & val)
    //{
    //    __GLOG_OUTPUT(val);
    //}
//private:
//    google::LogMessage * m_pLogMessage;
};
#endif // PUMP_USING_GLOG

class CTextLogRecordor
    : public CLogRecorderBase
{
public:
    CTextLogRecordor()
        : CLogRecorderBase(PUMP_CORE_LOG_RECORED_TEXT)
        , m_hFile(PUMP_INVALID_FILE)
    {

    }
    virtual ~CTextLogRecordor()
    {
    }
    virtual int Init(const PUMP_CORE_LOG_CONF & refConf)
    {
        return 0;
    }
    virtual int Destroy()
    {
        return 0;
    }
    virtual int WriteLine(CLogData & data)
    {
        if (m_struConf.emLogLevel > data.GetLevel())
        {
            return 0;
        }
        //va_list args;
        //va_start(args, szFormate);
        std::string strMsg;
        CLogData::__Time2Str(m_szTime, sizeof(m_szTime));
        pump_thread_id hThreadID = PUMP_CORE_Thread_GetSelfId();
        long tid = (long)(hThreadID);
        if (!m_struConf.bNotPrintFileInfo)
        {
            strMsg += data.GetSrcPos();
            pump_uint32_t line = data.GetSrcLine();
            strMsg += ":" + PUMP_CORE_Other2Str<pump_uint32_t>(line);
            strMsg += " <TID:" + PUMP_CORE_Other2Str(tid) + "> ";
            strMsg += m_szTime;
            strMsg += " { ";
            strMsg += data.GetMessage();
            strMsg += "\n";
        }
        else
        {
            strMsg = " <TID:" + PUMP_CORE_Other2Str(tid) + "> {\n" + data.GetMessage() + "\n}";
            strMsg = m_szTime + strMsg + "\n";
        }

        switch (data.GetLevel())
        {
        case PUMP_LOG_INFO:
            strMsg = "I " + strMsg;
            break;
        case PUMP_LOG_WARNING:
            strMsg = "W " + strMsg;
            break;
        case PUMP_LOG_ERROR:
        default:
            strMsg = "E " + strMsg;
            break;
        }
        if (m_struConf.bPrintConsole)
        {
            printf("%s", strMsg.c_str());
        }
        if (m_struConf.bWriteFile)
        {
            pump_bool_t bShouldLock = PUMP_TRUE;
            if (m_hFile == PUMP_INVALID_FILE)
            {
                m_csFile.Lock();
                bShouldLock = PUMP_FALSE;
                if (m_hFile == PUMP_INVALID_FILE)
                {
                    std::string strLogPath = m_struConf.szFilePath;
                    strLogPath += m_szTime;
                    strLogPath += ".log";
                    m_hFile = PUMP_CORE_FileOpen(strLogPath.c_str(), PUMP_CREATE | PUMP_WRITE, PUMP_ATTR_WRITE);
                    if (m_hFile == PUMP_INVALID_FILE)
                    {
                        printf("logger open file[%s] failed!\n", strLogPath);
                    }
                }
            }
            if (m_hFile != PUMP_INVALID_FILE)
            {
                if (bShouldLock)
                {
                    m_csFile.Lock();
                }
                pump_uint32_t dwSize = 0;
                PUMP_CORE_FileWrite(m_hFile, (void*)strMsg.c_str(), strMsg.size(), &dwSize);
            }
            m_csFile.Unlock();
        }
        return 0;
    }

    //static CTextLogRecordor * GetLogger();
public:
    pump_handle_t m_hFile;
    ::Pump::Core::Thread::CMutex m_csFile;
    char m_szTime[17];
};

class CUserLogRecordor
    : public CLogRecorderBase
{
public:
    CUserLogRecordor()
        : CLogRecorderBase(PUMP_CORE_LOG_RECORED_USER)
        , m_hFile(PUMP_INVALID_FILE)
    {
    }
    virtual ~CUserLogRecordor()
    {
    }
    virtual int Init(const PUMP_CORE_LOG_CONF & refConf)
    {
        m_csConf.Lock();
        m_struConf = refConf;
        m_csConf.Unlock();
        return 0;
    }
    virtual int Destroy()
    {
        return 0;
    }
    virtual int WriteLine(CLogData & data)
    {
        if (m_struConf.pfnLog)
        {
            m_struConf.pfnLog(
                (PUMP_CORE_LOG_LEVEL)data.GetLevel()
                , data.GetSrcPos()
                , data.GetSrcLine()
                , data.GetModular()
                , data.GetMessage()
                , data.GetMessageSize());
        }
        return 0;
    }

    //static CTextLogRecordor * GetLogger();
public:
    pump_handle_t m_hFile;
    ::Pump::Core::Thread::CMutex m_csFile;
    char m_szTime[17];
};

CLogRecorderMgr::CLogRecorderMgr()
{

}

CLogRecorderMgr::~CLogRecorderMgr()
{

}

CLogRecorderBase * CLogRecorderMgr::Create(PUMP_CORE_LOG_RECORED_TYPE emType)
{
    switch (emType)
    {
    case PUMP_CORE_LOG_RECORED_DEFAULT:
    case PUMP_CORE_LOG_RECORED_TEXT:
        return new (std::nothrow) CTextLogRecordor();
    case PUMP_CORE_LOG_RECORED_SQLITE:
    case PUMP_CORE_LOG_RECORED_GLOG:
#ifdef PUMP_USING_GLOG
        return new (std::nothrow) CGLogRecorder();
#endif // PUMP_USING_GLOG
    case PUMP_CORE_LOG_RECORED_USER:
        return new (std::nothrow) CUserLogRecordor();
    default:
        return PUMP_NULL;
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

CLogGuideBase::CLogGuideBase()
{}

CLogGuideBase::~CLogGuideBase()
{}

CPumpCoreLogGuide::CPumpCoreLogGuide()
{
}

CPumpCoreLogGuide::~CPumpCoreLogGuide()
{
}

pump_int32_t CPumpCoreLogGuide::WriteLine(
    PUMP_CORE_LOG_LEVEL emLevel,
    const char* szFile,
    unsigned int nLine,
    const char* szFormate,
    ...)
{
    std::string strMsg;
    CLogData logData;
    logData.SetLevel(emLevel);
    //logData.SetSrcPos(szFile);
    va_list argv;
    va_start(argv, szFormate);
    logData.SetMessage(emLevel, szFile, nLine, szFormate, argv);
    va_end(argv);
    __CPumpCoreLogRecorderKeeper logKeeper = ::Pump::Core::__CPumpCoreGlobalCtrl::GetLogger();
    if (!logKeeper.GetPtr())
    {
        return PUMP_ERROR;
    }
    return logKeeper.GetPtr()->WriteLine(logData);
}

}
}

//PUMP_CORE_API void PUMP_CALLBACK  __PUMP_CORE_Test_new_logger()
//{
//    //PUMP_CORE_LOG_CONF struLogCong;
//    //memset(&struLogCong, 0, sizeof(struLogCong));
//    //struLogCong.szFilePath = "";
//    //struLogCong.emLogLevel = PUMP_LOG_INFO;
//    //::Pump::Core::__CPumpCoreGlobalCtrl::SetLogger(struLogCong);
//    //__PUMP_CORE_INFO << "-------test begin-------";
//}

/**
 * pump core log init API.  
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_InjectLocalLogger(pump_handle_t hLogger)
{
    //::Pump::Core::CLoggerManagment::CreateLoggerManagment(pConf);
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    return ::Pump::Core::__CPumpCoreGlobalCtrl::SetLogger(hLogger);
}

PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_TYPE emType)
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
    return ((::Pump::Core::CLogRecorderBase *)hLog)->SetConfig(*pConf);
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
