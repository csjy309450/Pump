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
    //把单词输入字典树
    t.insert("proc/pid/mem", 10298);
    t.insert("proc/pid/meminfo", 2);
    t.insert("proc/pid/smaps", 3);
    t.insert("proc/pid/maps", 41);
    cout << "---树元素---" << endl;
    t.print_all();
    //查找测试
    cout << "---查找测试---" << endl;
    trie<int>::iterator it1 = t.find("proc/pid/mem");
    if (!it1.is_null())
    {
        cout << "存在 \"proc/pid/mem\"" << ", elem:" << (*it1).elem << endl;
    }
    else
    {
        cout << "不存在 \"proc/pid/mem\"" << endl;
    }

    trie<int>::iterator it2 = t.find("proc/pid/maps");
    if (!it2.is_null())
    {
        cout << "存在 \"proc/pid/maps\"" << ", elem:" << (*it2).elem << endl;
    }
    else
    {
        cout << "不存在 \"proc/pid/maps\"" << endl;
    }

    //删除测试
    cout << "---删除测试---" << endl;
    bool isDel = t.remove("proc/pid/mem");
    if (isDel)
    {
        cout << "删除 \"proc/pid/mem\" 成功!" << endl;
    }
    else
    {
        cout << "删除 \"proc/pid/mem\" 失败!" << endl;
    }
    cout << "---删除后树元素---" << endl;
    t.print_all();
    cout << "---输出以 \"proc/pid/s\" 开头的字符串---" << endl;
    //输出以proc开头的单词
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