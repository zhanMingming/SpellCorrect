#ifndef SPHCACHE_SPHMYTHREAD_H
#define SPHCACHE_SPHMYTHREAD_H

#include"Thread.h"
class ThreadPool;
class Thread;

class MyThread : public Thread
{
public:
    MyThread(ThreadPool &threadpool, int type);
    void run();

private:
    ThreadPool &m_threadpool;
    int m_type;
};


#endif

