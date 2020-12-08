//
// Created by yz on 19-8-4.
//
#include <iostream>
#include <fstream>
#include <algorithm>
#include "pump_memory/trie.hpp"
#include "pump_memory/smart_buffer/smart_buffer.h"

using namespace std;
using namespace Pump::Memory;

int test_trie()
{
    trie<int> t;
    //�ѵ��������ֵ���
    t.insert("proc/pid/mem", 10298);
    t.insert("proc/pid/meminfo", 2);
    t.insert("proc/pid/smaps", 3);
    t.insert("proc/pid/maps", 41);
    cout << "---��Ԫ��---" << endl;
    t.print_all();
    //���Ҳ���
    cout << "---���Ҳ���---" << endl;
    trie<int>::iterator it1 = t.find("proc/pid/mem");
    if (!it1.is_null())
    {
        cout << "���� \"proc/pid/mem\"" << ", elem:" << (*it1).elem << endl;
    }
    else
    {
        cout << "������ \"proc/pid/mem\"" << endl;
    }

    trie<int>::iterator it2 = t.find("proc/pid/maps");
    if (!it2.is_null())
    {
        cout << "���� \"proc/pid/maps\"" << ", elem:" << (*it2).elem << endl;
    }
    else
    {
        cout << "������ \"proc/pid/maps\"" << endl;
    }

    //ɾ������
    cout << "---ɾ������---" << endl;
    bool isDel = t.remove("proc/pid/mem");
    if (isDel)
    {
        cout << "ɾ�� \"proc/pid/mem\" �ɹ�!" << endl;
    }
    else
    {
        cout << "ɾ�� \"proc/pid/mem\" ʧ��!" << endl;
    }
    cout << "---ɾ������Ԫ��---" << endl;
    t.print_all();
    cout << "---����� \"proc/pid/s\" ��ͷ���ַ���---" << endl;
    //�����proc��ͷ�ĵ���
    t.print_pre("proc/pid/s");
    cout << endl;
    return 0;
}
int test_smart_buffer()
{
    const char * szMsg = "AAAAAAAAAAAAAAAAAAAAAAAAA";
    smart_buffer<char> buf(8,NULL);
    buf.append(szMsg, strlen(szMsg));
    return 0;
}

int main()
{
    test_smart_buffer();
    return getchar();
}