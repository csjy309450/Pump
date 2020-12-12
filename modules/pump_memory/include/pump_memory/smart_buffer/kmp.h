/**
 * @file KMP.h
 * @brief 定义 libpump 库的 IO 缓冲区模块, 字符串匹配算法
 *
 * @author YangZheng 263693992@qq.com
 * @version 1.0
 * @date 2018.08.12
 */

#ifndef LIBPUMP_KMP_H
#define LIBPUMP_KMP_H

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <string>
#include <deque>
#include <vector>

#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Memory
{

class kmp
{
public:
  kmp();
  
  ~kmp();
  
  long apply(const char *szSrc, long iSrcLen, const char *szPattern, long iPLen);
  
  long apply(const std::deque<char *> &vBuf, long iSrcLen, const char *szPattern, long iPLen);
  
  long apply(char **pszSrc, long iSLen, int iSCount, const char *szPattern, long iPLen);

private:
  void cal_next(const char *szPattern, long iPLen);
  
  std::vector<int> m_vNext;
};

}
}
#endif //LIBPUMP_KMP_H
