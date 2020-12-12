//
// Created by yz on 19-8-5.
//

#ifndef DRONE_SIMPLE_BUFFER_HPP
#define DRONE_SIMPLE_BUFFER_HPP

#include <cstdio>
#include <vector>
#include <string>

namespace Pump
{
namespace Memory
{

class simple_buffer
{
public:
  simple_buffer()
  {
  
  }
  
  void append(const char *data)
  {
    if (data == NULL)
    {
      return;
    }
    if(m_buf.empty())
    {
      m_buf.push_back(std::string(data));
    }
    else if(m_buf.back().size()==std::string().max_size())
    {
      m_buf.push_back(std::string(data));
    }
    else
    {
      m_buf.back().append(data);
    }
  }
  
  void assign(const char *data)
  {
    if (data == NULL)
    {
      return;
    }
    if(m_buf.empty())
    {
      m_buf.push_back(std::string(data));
    }
    else
    {
      m_buf.back().assign(data);
    }
  }
  
  ~simple_buffer()
  {
  }
  
  void print() const
  {
    for (int i=0;i<m_buf.size();++i)
    {
      printf("%s", m_buf[i].c_str());
      if(i+1==m_buf.size())
      {
        printf("\n");
      }
    }
  }
  
  std::string std_str() const
  {
    if(m_buf.empty())
      return "";
    return m_buf.front();
  }
  
  ssize_t size()
  {
    ssize_t l_size = 0;
    for (int i = 0; i < m_buf.size(); ++i)
    {
      l_size += m_buf[i].size();
    }
  }
  
  void clear()
  {
    m_buf.clear();
  }

private:
  std::vector<std::string> m_buf;
};

}
}

#endif //DRONE_SIMPLE_BUFFER_HPP
