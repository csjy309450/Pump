//
// Created by yz on 19-8-4.
//

#ifndef DRONE_TRIE_H
#define DRONE_TRIE_H

#include <cstring>
#include <iostream>
#include <string>
#include <cassert>

namespace Pump
{
namespace Memory
{

const int MaxBranchNum = 256;//如果区分大小写，可以扩展到52

template<class _Type>
class trie;

/*定义trie树结点*/
template<class _Type>
class _trie_node
{
  friend class trie<_Type>;
public:
  typedef _Type elmemt_type;
private:
  char *word; //节点表示的单词
  int count;  //单词出现的次数
  _trie_node *nextBranch[MaxBranchNum];//指向26个字符节点的指针
public:
  elmemt_type elem;
public:
  _trie_node()
    : word(NULL), count(0), elem()
  {
    memset(nextBranch, 0, sizeof(_trie_node *) * MaxBranchNum);
  }
};

/*定义类Trie*/
template<class _Type>
class trie
{
public:
  typedef _Type type_elmemt;
  typedef _trie_node<type_elmemt> node_type;
  
  class iterator
  {
    friend class trie;
  
  public:
    typedef _trie_node<_Type> node_type;
  public:
    iterator(const iterator &other)
    {
      p_node = other.p_node;
    }
    
    iterator()
      : p_node(NULL) {}
    
    bool operator==(const iterator &right)
    {
      return p_node == right.p_node;
    }
  
    node_type & operator*()
    {
      return *p_node;
    }
    
    bool is_null()
    {
      return p_node == NULL;
    }
  
    node_type * get()
    {
      return p_node;
    }
  
  private:
    explicit iterator(node_type *_p_node)
    {
      p_node = _p_node;
    }
  
  private:
    node_type *p_node;
  };

public:
  trie()
  {
    pRoot = new node_type();//注意字典树的根不存放字符
  }
  
  ~trie()
  {
    destory(pRoot);
  }
  
  void insert(const char *str, const type_elmemt &elem)//插入字符串str
  {
    assert(NULL != str);
    int index;
    node_type *pLoc = pRoot;
    for (int i = 0; str[i]; i++)
    {
      index = str[i]; //- 'a';//如果区分大小写，可以扩展
      
      if (index < 0 || index > MaxBranchNum)//不执行插入
      {
        return;
      }
      
      if (NULL == pLoc->nextBranch[index])//该单词的前缀不存在，要生成该结点
      {
        pLoc->nextBranch[index] = new node_type();
      }
      pLoc = pLoc->nextBranch[index];
    }
    if (NULL != pLoc->word)//单词已经出现过
    {
      pLoc->count++;
    } else    //单词没有出现过，应该插入单词
    {
      pLoc->count++;
      pLoc->word = new char[strlen(str) + 1];
      assert(NULL != pLoc->word);
      strcpy(pLoc->word, str);
    }
    pLoc->elem = elem;
  }
  
  iterator find(const char *str)//查找字符串str，并返回出现的次数
  {
    assert(str != NULL);
    int i = 0;
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//如果区分大小写，可以扩展
      
      if (index < 0 || index > MaxBranchNum)//不是一个单词，不执行插入
      {
        return trie::iterator();
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc && pLoc->word)//条件成立，找到该单词
    {
      return trie::iterator(pLoc);
    }
    return trie::iterator();
  }
  
  bool remove(const char *str)//删除字符串str
  {
    assert(NULL != str);
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//如果区分大小写，可以扩展
      
      if (index < 0 || index > MaxBranchNum)//不是一个单词，不执行插入
      {
        return false;
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc && pLoc->word)//条件成立，找到该单词
    {
      delete[] pLoc->word;
      pLoc->word = NULL;
      return true;
    }
    return false;
  }
  
  void print_all()//打印trie树中所有的结点
  {
    print(pRoot);
  }
  
  void print_pre(const char *str)//打印以str为前缀的单词
  {
    assert(str != NULL);
    int i = 0;
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//如果区分大小写，可以扩展
      
      if (index < 0 || index > MaxBranchNum)//不是一个单词，不执行插入
      {
        return;
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc)//条件成立，找到该单词
    {
      print(pLoc);
    }
  }

private:
  void destory(node_type *pRoot)
  {
    if (NULL == pRoot)
    {
      return;
    }
    for (int i = 0; i < MaxBranchNum; i++)
    {
      destory(pRoot->nextBranch[i]);
    }
    //销毁单词占得空间
    if (NULL != pRoot->word)
    {
      delete[]pRoot->word;
      pRoot->word = NULL;
    }
    delete pRoot;//销毁结点
  }
  
  void print(node_type *pRoot)
  {
    if (NULL == pRoot)
    {
      return;
    }
    //输出单词
    if (NULL != pRoot->word)
    {
      std::cout << pRoot->word << " " << pRoot->count << std::endl;
    }
    //递归处理分支
    for (int i = 0; i < MaxBranchNum; i++)
    {
      print(pRoot->nextBranch[i]);
    }
  }

private:
  node_type *pRoot;
};

}
}

#endif //DRONE_TRIE_H