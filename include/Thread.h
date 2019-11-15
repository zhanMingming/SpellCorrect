#ifndef SPACACHE_SPHTHREAD_H
#define SPACACHE_SPHTHREAD_H

#include<pthread.h>
#include"NoneCopyable.h"

class Thread : private NoneCopyable
{
public:
    Thread();
    ~Thread();
    
    void start();
    void join();
    virtual void run() = 0;
    static void * runInThread(void *arg);


private:
    pthread_t m_pthId;
    bool     m_isRunning;
};

#endif

  
