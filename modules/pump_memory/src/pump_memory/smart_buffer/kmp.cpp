#include "pump_memory/smart_buffer/kmp.h"

namespace Pump
{
namespace Memory
{

kmp::kmp()
{
}


kmp::~kmp()
{
}

void kmp::cal_next(const char *szPattern, const long iLen)
{
  m_vNext.clear();
  m_vNext.push_back(-1);// next[0]初始化为-1，-1表示不存在相同的最大前缀和最大后缀
  int k = -1; //k初始化为-1
  for (int q = 1; q <= iLen - 1; q++)
  {
    while (k > -1 && szPattern[k + 1] != szPattern[q])//如果下一个不同，那么k就变成next[k]，注意next[k]是小于k的，无论k取任何值。
    {
      k = m_vNext[k]; // 往前回溯
    }
    if (szPattern[k + 1] == szPattern[q])//如果相同，k++
    {
      k = k + 1;
    }
    m_vNext.push_back(k); //这个是把算的k的值（就是相同的最大前缀和最大后缀长）赋给next[q]
  }
}

long kmp::apply(const char *szSrc, long iSrcLen, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);
  int k = -1;
  for (int i = 0; i < iSrcLen; i++)
  {
    while (k > -1 && szPattern[k + 1] != szSrc[i])//ptr和str不匹配，且k>-1（表示ptr和str有部分匹配）
      k = m_vNext[k];
    if (szPattern[k + 1] == szSrc[i])
      k = k + 1;
    if (k == iPLen - 1) //说明k移动到ptr的最末端
    {
      return i - iPLen + 1;
    }
  }
  return -1;
}

long kmp::apply(char **pszSrc, long iSLen, int iSCount, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);//计算next数组
  int k = -1;
  for (int i = 0; i < iSLen * iSCount; i++)
  {
    while (k > -1 && szPattern[k + 1] != pszSrc[i / iSLen][i % iSLen])//ptr和str不匹配，且k>-1（表示ptr和str有部分匹配）
      k = m_vNext[k];//往前回溯
    if (szPattern[k + 1] == pszSrc[i / iSLen][i % iSLen])
      k = k + 1;
    if (k == iPLen - 1)//说明k移动到ptr的最末端
    {
      //cout << "在位置" << i-plen+1<< endl;
      //k = -1;//重新初始化，寻找下一个
      //i = i - plen + 1;//i定位到该位置，外层for循环i++可以继续找下一个（这里默认存在两个匹配字符串可以部分重叠），感谢评论中同学指出错误。
      return i - iPLen + 1;//返回相应的位置
    }
  }
  return -1;
}

long kmp::apply(const std::deque<char *> &vBuf, long iSrcLen, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);//计算next数组
  int k = -1;
  for (size_t i = 0; i < vBuf.size() * iSrcLen; i++)
  {
    while (k > -1 && szPattern[k + 1] != vBuf[i / iSrcLen][i % iSrcLen])//ptr和str不匹配，且k>-1（表示ptr和str有部分匹配）
      k = m_vNext[k];//往前回溯
    if (szPattern[k + 1] == vBuf[i / iSrcLen][i % iSrcLen])
      k = k + 1;
    if (k == iPLen - 1)//说明k移动到ptr的最末端
    {
      //cout << "在位置" << i-plen+1<< endl;
      //k = -1;//重新初始化，寻找下一个
      //i = i - plen + 1;//i定位到该位置，外层for循环i++可以继续找下一个（这里默认存在两个匹配字符串可以部分重叠），感谢评论中同学指出错误。
      return i - iPLen + 1; //返回相应的位置
    }
  }
  return -1;
}

}
}