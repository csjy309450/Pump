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
#include <iostream>
#include <ostream>
#include <string>
#include <cassert>
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_global_resouce_guider.hpp"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/thread/pump_core_mutex.h"


typedef enum tagPUMP_CORE_LOG_LEVEL
{
    PUMP_LOG_INFO = 0,
    PUMP_LOG_WARNING,
    PUMP_LOG_ERROR
} PUMP_CORE_LOG_LEVEL;

typedef enum tagPUMP_CORE_LOG_RECORED_TYPE
{
    PUMP_CORE_LOG_RECORED_DEFAULT = 0,
    PUMP_CORE_LOG_RECORED_TEXT,
    PUMP_CORE_LOG_RECORED_GLOG,
    PUMP_CORE_LOG_RECORED_SQLITE,
} PUMP_CORE_LOG_RECORED_TYPE;

typedef struct tagPUMP_CORE_LOG_CONF
{
    PUMP_CORE_LOG_LEVEL emLogLevel;
    pump_bool_t bPrintConsole;
    pump_bool_t bNotPrintFileInfo;
    pump_bool_t bWriteFile;
    char szFilePath[260];
}PUMP_CORE_LOG_CONF, *LPPUMP_CORE_LOG_CONF;

/** Inject logger to pump_core */
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_InjectLocalLogger(pump_handle_t hLogger);

//PUMP_CORE_API void PUMP_CALLBACK  __PUMP_CORE_Test_new_logger();

/** [add] yangzheng 20200702 add multi log recorder*/
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_TYPE emType = PUMP_CORE_LOG_RECORED_DEFAULT);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerDestroy(pump_handle_t hLog);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerConfig(pump_handle_t hLog, const LPPUMP_CORE_LOG_CONF pConf);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_LoggerWrite(pump_handle_t hLog, const char* szFormat, ...);

#define PUMP_CORE_INFO(form_, ...) ::Pump::Core::CLogGuide().WriteLine(PUMP_LOG_INFO, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_CORE_WARING(form_, ...) ::Pump::Core::CLogGuide().WriteLine(PUMP_LOG_WARNING, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_CORE_ERR(form_, ...) ::Pump::Core::CLogGuide().WriteLine(PUMP_LOG_ERROR, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_CORE_LOG_ASSERT(exp_,form_, ...) assert(exp_);PUMP_CORE_INFO(form_, __VA_ARGS__)

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CLogData
{
public:
    CLogData();
    virtual ~CLogData();
    CLogData(const CLogData & other);
    CLogData & operator=(const CLogData & other);
    void SetTm(long long val);
    void ResetTm();
    long long GetTm();
    void SetTID(unsigned  long long val);
    void ResetTID();
    unsigned  long long GetTID();
    void SetLevel(unsigned char val);
    void ResetLevel();
    unsigned char GetLevel();
    void SetModular(const char* val);
    void ResetModular();
    const char* GetModular();
    void SetSrcPos(const char* val);
    void ResetSrcPos();
    const char* GetSrcPos();
    void SetSrcLine(pump_uint32_t val);
    void ResetSrcLine();
    pump_uint32_t GetSrcLine();
    void SetMessage(
        PUMP_CORE_LOG_LEVEL emLevel,
        const char* szFile,
        unsigned int nLine,
        const char* szFormate,
        va_list args);
    void ResetMessage();
    const char * GetMessage();

public:
    static void __Time2Str(char * pBuf, size_t dwSize);

private:
    long long m_tm;
    unsigned  long long m_tid;
    unsigned char m_byLevel;
    char m_strModular[64];
    char m_strSrcPos[256];
    pump_uint32_t m_nSrcLine;
    std::string m_strMsg;
};

/*
 * @brief The base class of log recorder, writing log message to file.
 */
class PUMP_CORE_CLASS CLogRecorderBase
{
public:
    CLogRecorderBase();
    explicit CLogRecorderBase(PUMP_CORE_LOG_RECORED_TYPE emType);
    virtual ~CLogRecorderBase();

public:
    /**
     * @brief Recordor对象的初始化接口，派生类必须实现。
     *              调用者必须保证，不重复调用
     * @param refConf [in] 配置
     * @return 0-成功，-1-失败.
     */
    virtual int Init(const PUMP_CORE_LOG_CONF & refConf) = 0;
    inline pump_bool_t IsInit() const { return m_bInit; }
    /**
     * @brief Recordor对象的反初始化接口，派生类必须实现。
     *              调用者必须保证，不重复调用
     * @return 0-成功，-1-失败.
     */
    virtual int Destroy() = 0;
    virtual pump_int32_t SetConfig(const PUMP_CORE_LOG_CONF & refConf);
    virtual PUMP_CORE_LOG_CONF GetConfig() const;
    virtual int WriteLine(CLogData & data) = 0;

    // @{ [deprecated] after 20201205
    //virtual CLogRecorderBase* Begin(const char* szFile, int nLine, PUMP_CORE_LOG_LEVEL emLogLevel) { return this; }
    //virtual void End() {}
    //virtual CLogRecorderBase* operator<< (bool val) = 0;
    //virtual CLogRecorderBase* operator<< (short val) = 0;
    //virtual CLogRecorderBase* operator<< (char val) = 0;
    //virtual CLogRecorderBase* operator<< (unsigned short val) = 0;
    //virtual CLogRecorderBase* operator<< (int val) = 0;
    //virtual  CLogRecorderBase* operator<< (unsigned int val) = 0;
    //virtual  CLogRecorderBase* operator<< (long val) = 0;
    //virtual CLogRecorderBase* operator<< (unsigned long val) = 0;
    //virtual CLogRecorderBase* operator<< (float val) = 0;
    //virtual CLogRecorderBase* operator<< (double val) = 0;
    //virtual CLogRecorderBase* operator<< (long double val) = 0;
    //virtual CLogRecorderBase* operator<< (void* val) = 0;
    //virtual CLogRecorderBase* operator<< (const char* val) = 0;
    //virtual CLogRecorderBase* operator<< (const std::string & val) = 0;
    // @}

protected:
    pump_bool_t m_bInit;
    ::Pump::Core::Thread::CMutex m_csInit;

    PUMP_CORE_LOG_CONF m_struConf;
    ::Pump::Core::Thread::CMutex m_csConf;

    const PUMP_CORE_LOG_RECORED_TYPE m_emType;
};

/*
 * @brief Management the log recorder handlers. 
 */
class PUMP_CORE_CLASS CLogRecorderMgr
{
public:
    CLogRecorderMgr();
    ~CLogRecorderMgr();
    CLogRecorderBase * Create(PUMP_CORE_LOG_RECORED_TYPE emType);
    pump_int32_t Destroy(CLogRecorderBase * pLogRecorder);
};

class PUMP_CORE_CLASS CLogRecorderKeeper
    : public ::Pump::Core::CGlobalResouceKeeper<CLogRecorderBase>
{
public:
    CLogRecorderKeeper(CLogRecorderBase* pLogRecorder);
    CLogRecorderKeeper(CLogRecorderKeeper & other);
    virtual ~CLogRecorderKeeper();
};

class PUMP_CORE_CLASS CLogGuide
    : public CNonCopyable
{
public:
    CLogGuide();
    ~CLogGuide();

public:
    pump_int32_t WriteLine(
        PUMP_CORE_LOG_LEVEL emLevel, 
        const char* szFile, 
        unsigned int nLine,
        const char* szFormate, 
        ...);
    //CLogGuide& operator<< (bool val);
    //CLogGuide& operator<< (short val);
    //CLogGuide& operator<< (char val);
    //CLogGuide& operator<< (unsigned short val);
    //CLogGuide& operator<< (int val);
    //CLogGuide& operator<< (unsigned int val);
    //CLogGuide& operator<< (long val);
    //CLogGuide& operator<< (unsigned long val);
    //CLogGuide& operator<< (float val);
    //CLogGuide& operator<< (double val);
    //CLogGuide& operator<< (long double val);
    //CLogGuide& operator<< (void* val);
    //CLogGuide& operator<< (const char* val);
    //CLogGuide& operator<< (const std::string & val);
};

}
}

#endif //PUMP_CORE_LOGGER_H
