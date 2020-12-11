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
  m_vNext.push_back(-1);// next[0]��ʼ��Ϊ-1��-1��ʾ��������ͬ�����ǰ׺������׺
  int k = -1; //k��ʼ��Ϊ-1
  for (int q = 1; q <= iLen - 1; q++)
  {
    while (k > -1 && szPattern[k + 1] != szPattern[q])//�����һ����ͬ����ôk�ͱ��next[k]��ע��next[k]��С��k�ģ�����kȡ�κ�ֵ��
    {
      k = m_vNext[k]; // ��ǰ����
    }
    if (szPattern[k + 1] == szPattern[q])//�����ͬ��k++
    {
      k = k + 1;
    }
    m_vNext.push_back(k); //����ǰ����k��ֵ��������ͬ�����ǰ׺������׺��������next[q]
  }
}

long kmp::apply(const char *szSrc, long iSrcLen, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);
  int k = -1;
  for (int i = 0; i < iSrcLen; i++)
  {
    while (k > -1 && szPattern[k + 1] != szSrc[i])//ptr��str��ƥ�䣬��k>-1����ʾptr��str�в���ƥ�䣩
      k = m_vNext[k];
    if (szPattern[k + 1] == szSrc[i])
      k = k + 1;
    if (k == iPLen - 1) //˵��k�ƶ���ptr����ĩ��
    {
      return i - iPLen + 1;
    }
  }
  return -1;
}

long kmp::apply(char **pszSrc, long iSLen, int iSCount, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);//����next����
  int k = -1;
  for (int i = 0; i < iSLen * iSCount; i++)
  {
    while (k > -1 && szPattern[k + 1] != pszSrc[i / iSLen][i % iSLen])//ptr��str��ƥ�䣬��k>-1����ʾptr��str�в���ƥ�䣩
      k = m_vNext[k];//��ǰ����
    if (szPattern[k + 1] == pszSrc[i / iSLen][i % iSLen])
      k = k + 1;
    if (k == iPLen - 1)//˵��k�ƶ���ptr����ĩ��
    {
      //cout << "��λ��" << i-plen+1<< endl;
      //k = -1;//���³�ʼ����Ѱ����һ��
      //i = i - plen + 1;//i��λ����λ�ã����forѭ��i++���Լ�������һ��������Ĭ�ϴ�������ƥ���ַ������Բ����ص�������л������ͬѧָ������
      return i - iPLen + 1;//������Ӧ��λ��
    }
  }
  return -1;
}

long kmp::apply(const std::deque<char *> &vBuf, long iSrcLen, const char *szPattern, long iPLen)
{
  cal_next(szPattern, iPLen);//����next����
  int k = -1;
  for (size_t i = 0; i < vBuf.size() * iSrcLen; i++)
  {
    while (k > -1 && szPattern[k + 1] != vBuf[i / iSrcLen][i % iSrcLen])//ptr��str��ƥ�䣬��k>-1����ʾptr��str�в���ƥ�䣩
      k = m_vNext[k];//��ǰ����
    if (szPattern[k + 1] == vBuf[i / iSrcLen][i % iSrcLen])
      k = k + 1;
    if (k == iPLen - 1)//˵��k�ƶ���ptr����ĩ��
    {
      //cout << "��λ��" << i-plen+1<< endl;
      //k = -1;//���³�ʼ����Ѱ����һ��
      //i = i - plen + 1;//i��λ����λ�ã����forѭ��i++���Լ�������һ��������Ĭ�ϴ�������ƥ���ַ������Բ����ص�������л������ͬѧָ������
      return i - iPLen + 1; //������Ӧ��λ��
    }
  }
  return -1;
}

}
}