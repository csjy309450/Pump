/**
 * @file pump_core_logger.h
 * @brief Library Header.
 * @details Declaration of log control interface.
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

#ifndef PUMP_CORE_LOGGER_H
#define PUMP_CORE_LOGGER_H
#include "pump_macro/pump_pre.h"
//#include <stdlib.h>
//#include <ctype.h>
#include <ostream>
#include <string>
#include <cassert>
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/pump_core_types.h"

enum PUMP_CORE_LOG_LEVEL
{
    PUMP_LOG_INFO = 0,
    PUMP_LOG_WARNING,
    PUMP_LOG_ERROR
};

typedef enum tagPUMP_LOG_RECORED_TYPE
{
    PUMP_LOG_RECORED_DEFAULT = 0,
    PUMP_LOG_RECORED_GLOG,
} PUMP_LOG_RECORED_TYPE;

typedef struct tagPUMP_CORE_LOG_CONF
{
    const char* szFilePath;
    enum PUMP_CORE_LOG_LEVEL emLogLevel;
}PUMP_CORE_LOG_CONF, *LPPUMP_CORE_LOG_CONF;

PUMP_CORE_API void PUMP_CALLBACK PUMP_CORE_InitLogger(LPPUMP_CORE_LOG_CONF pConf);

PUMP_CORE_API void PUMP_CALLBACK  __PUMP_CORE_Test_new_logger();

/** [add] yangzheng 20200702 add multi log recorder*/
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_CreateLogger(PUMP_LOG_RECORED_TYPE emType);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DestroyLogger(pump_handle_t hLog);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SetLogger(pump_handle_t hLog, const LPPUMP_CORE_LOG_CONF pConf);
PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_WriteLog(pump_handle_t hLog, const char* szFormat, ...);

namespace Pump
{
namespace Core
{

/*
 * @brief deprecated after 20200703
 */
class PUMP_CORE_CLASS CLogger
  : public CNonCopyable
{
public:
  CLogger(const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel);
  
  ~CLogger();

public:
  CLogger& operator<< (bool val);
  CLogger& operator<< (short val);
  CLogger& operator<< (char val);
  CLogger& operator<< (unsigned short val);
  CLogger& operator<< (int val);
  CLogger& operator<< (unsigned int val);
  CLogger& operator<< (long val);
  CLogger& operator<< (unsigned long val);
  CLogger& operator<< (float val);
  CLogger& operator<< (double val);
  CLogger& operator<< (long double val);
  CLogger& operator<< (void* val);
  CLogger& operator<< (const char* val);
  CLogger& operator<< (const std::string & val);

private:
  void * m_pLogMessage;
};

/*
 * @brief The base class of log recorder, writing log message to file.
 */
class PUMP_CORE_CLASS CLogRecorderBase
{
public:
    CLogRecorderBase() {}
    virtual ~CLogRecorderBase() {}

public:
    virtual pump_int32_t Set(const PUMP_CORE_LOG_CONF & struConf)
    {
        m_csConf.Lock();
        m_struConf = struConf;
        m_csConf.Unlock();
        return PUMP_OK;
    }
    virtual CLogRecorderBase* Begin(const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel) { return this; }
    virtual void End() {}
    virtual CLogRecorderBase* operator<< (bool val) = 0;
    virtual CLogRecorderBase* operator<< (short val) = 0;
    virtual CLogRecorderBase* operator<< (char val) = 0;
    virtual CLogRecorderBase* operator<< (unsigned short val) = 0;
    virtual CLogRecorderBase* operator<< (int val) = 0;
    virtual  CLogRecorderBase* operator<< (unsigned int val) = 0;
    virtual  CLogRecorderBase* operator<< (long val) = 0;
    virtual CLogRecorderBase* operator<< (unsigned long val) = 0;
    virtual CLogRecorderBase* operator<< (float val) = 0;
    virtual CLogRecorderBase* operator<< (double val) = 0;
    virtual CLogRecorderBase* operator<< (long double val) = 0;
    virtual CLogRecorderBase* operator<< (void* val) = 0;
    virtual CLogRecorderBase* operator<< (const char* val) = 0;
    virtual CLogRecorderBase* operator<< (const std::string & val) = 0;

protected:
    PUMP_CORE_LOG_CONF m_struConf;
    ::Pump::Core::Thread::CMutex m_csConf;
};

/*
 * @brief Management the log recorder handlers. 
 */
class PUMP_CORE_CLASS CLogRecorderMgr
{
public:
    CLogRecorderMgr();
    ~CLogRecorderMgr();
    CLogRecorderBase * Create(PUMP_LOG_RECORED_TYPE emType);
    pump_int32_t Destroy(CLogRecorderBase * pLogRecorder);
};

class PUMP_CORE_CLASS CLogGuide
    : public CNonCopyable
{
public:
    CLogGuide(CLogRecorderBase * const pLogRecorder, const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel);

    ~CLogGuide();

public:
    CLogGuide& operator<< (bool val);
    CLogGuide& operator<< (short val);
    CLogGuide& operator<< (char val);
    CLogGuide& operator<< (unsigned short val);
    CLogGuide& operator<< (int val);
    CLogGuide& operator<< (unsigned int val);
    CLogGuide& operator<< (long val);
    CLogGuide& operator<< (unsigned long val);
    CLogGuide& operator<< (float val);
    CLogGuide& operator<< (double val);
    CLogGuide& operator<< (long double val);
    CLogGuide& operator<< (void* val);
    CLogGuide& operator<< (const char* val);
    CLogGuide& operator<< (const std::string & val);

private:
    CLogRecorderBase * const m_pLogRecorder;
};

}
}

#define PUMP_CORE_INFO ::Pump::Core::CLogger(__FILE__,__LINE__,PUMP_LOG_INFO)
#define PUMP_CORE_WARING ::Pump::Core::CLogger(__FILE__,__LINE__,PUMP_LOG_WARNING)
#define PUMP_CORE_ERR ::Pump::Core::CLogger(__FILE__,__LINE__,PUMP_LOG_ERROR)
#define PUMP_CORE_LOG_ASSERT(exp) assert(exp);PUMP_CORE_INFO

#endif //PUMP_CORE_LOGGER_H
