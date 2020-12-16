//
// Created by yz on 19-6-26.
//
#ifdef __linux__
#include <pthread.h>
#endif // __linux__

#include <cstdio>
#include <vector>
#include "pump_core/pump_core_rwlocker.h"

using namespace std;

Pump::Core::Thread::CRWLocker g_rwMux;

void* threadCallback_reader(void * data)
{
  long num = (long)data;
  for(int i=0;i<100;i++)
  {
    g_rwMux.readLock();
    printf("reader %ld\n", num*100+i);
    g_rwMux.readUnlock();
  }
  return NULL;
}

void* threadCallback_writer(void * data)
{
  long num = (long) data;
  for (int i = 0; i < 100; i++)
  {
    g_rwMux.writeLock();
    printf("writer %ld\n", num * 100 + i);
    g_rwMux.writeUnlock();
  }
  return NULL;
}

void test_0()
{
    for (long i = 0; i < 10; i++)
    {
#ifdef __linux__
      pthread_t tid;
      if (i % 2 == 0)
      {
          pthread_create(&tid, NULL, threadCallback_writer, (void*)i);
      }
      else{
          pthread_create(&tid, NULL, threadCallback_reader, (void*)i);
      }

      pthread_detach(tid);
#endif // __linux__ 
    }
}

int main(int argc, char** argv)
{
  test_0();
  return getchar();
}