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

const int MaxBranchNum = 256;//������ִ�Сд��������չ��52

template<class _Type>
class trie;

/*����trie�����*/
template<class _Type>
class _trie_node
{
  friend class trie<_Type>;
public:
  typedef _Type elmemt_type;
private:
  char *word; //�ڵ��ʾ�ĵ���
  int count;  //���ʳ��ֵĴ���
  _trie_node *nextBranch[MaxBranchNum];//ָ��26���ַ��ڵ��ָ��
public:
  elmemt_type elem;
public:
  _trie_node()
    : word(NULL), count(0), elem()
  {
    memset(nextBranch, 0, sizeof(_trie_node *) * MaxBranchNum);
  }
};

/*������Trie*/
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
    pRoot = new node_type();//ע���ֵ����ĸ�������ַ�
  }
  
  ~trie()
  {
    destory(pRoot);
  }
  
  void insert(const char *str, const type_elmemt &elem)//�����ַ���str
  {
    assert(NULL != str);
    int index;
    node_type *pLoc = pRoot;
    for (int i = 0; str[i]; i++)
    {
      index = str[i]; //- 'a';//������ִ�Сд��������չ
      
      if (index < 0 || index > MaxBranchNum)//��ִ�в���
      {
        return;
      }
      
      if (NULL == pLoc->nextBranch[index])//�õ��ʵ�ǰ׺�����ڣ�Ҫ���ɸý��
      {
        pLoc->nextBranch[index] = new node_type();
      }
      pLoc = pLoc->nextBranch[index];
    }
    if (NULL != pLoc->word)//�����Ѿ����ֹ�
    {
      pLoc->count++;
    } else    //����û�г��ֹ���Ӧ�ò��뵥��
    {
      pLoc->count++;
      pLoc->word = new char[strlen(str) + 1];
      assert(NULL != pLoc->word);
      strcpy(pLoc->word, str);
    }
    pLoc->elem = elem;
  }
  
  iterator find(const char *str)//�����ַ���str�������س��ֵĴ���
  {
    assert(str != NULL);
    int i = 0;
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//������ִ�Сд��������չ
      
      if (index < 0 || index > MaxBranchNum)//����һ�����ʣ���ִ�в���
      {
        return trie::iterator();
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc && pLoc->word)//�����������ҵ��õ���
    {
      return trie::iterator(pLoc);
    }
    return trie::iterator();
  }
  
  bool remove(const char *str)//ɾ���ַ���str
  {
    assert(NULL != str);
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//������ִ�Сд��������չ
      
      if (index < 0 || index > MaxBranchNum)//����һ�����ʣ���ִ�в���
      {
        return false;
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc && pLoc->word)//�����������ҵ��õ���
    {
      delete[] pLoc->word;
      pLoc->word = NULL;
      return true;
    }
    return false;
  }
  
  void print_all()//��ӡtrie�������еĽ��
  {
    print(pRoot);
  }
  
  void print_pre(const char *str)//��ӡ��strΪǰ׺�ĵ���
  {
    assert(str != NULL);
    int i = 0;
    int index = -1;;
    node_type *pLoc = pRoot;
    while (pLoc && *str)
    {
      index = *str;// - 'a';//������ִ�Сд��������չ
      
      if (index < 0 || index > MaxBranchNum)//����һ�����ʣ���ִ�в���
      {
        return;
      }
      
      pLoc = pLoc->nextBranch[index];
      str++;
    }
    if (pLoc)//�����������ҵ��õ���
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
    //���ٵ���ռ�ÿռ�
    if (NULL != pRoot->word)
    {
      delete[]pRoot->word;
      pRoot->word = NULL;
    }
    delete pRoot;//���ٽ��
  }
  
  void print(node_type *pRoot)
  {
    if (NULL == pRoot)
    {
      return;
    }
    //�������
    if (NULL != pRoot->word)
    {
      std::cout << pRoot->word << " " << pRoot->count << std::endl;
    }
    //�ݹ鴦���֧
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