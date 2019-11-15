#ifndef SPHCACHE_SPHTHREADPOOL_H
#define SPHCACHE_SPHTHREADPOOL_H
#include<climits>
#include<vector>
#include"common.h"
#include"Buffer.h"
#include"Epoller.h"
#include"Cache.h"
#include"Query.h"

class MyThread;
class Thread;

struct ThreadType
{
    enum {
        WorkerThread = 1,
        SyncThread
    };
};

class ThreadPool
{
public:
    ThreadPool(Epoller &epoll);
    ~ThreadPool();
    
    void start();
    void stop();
    void doWorker();
    void doSync();
    void add(int fd);
private:
    Buffer<int>                      m_queFd;
    std::vector<Thread *>            m_vecThreads;
    Epoller                          &m_epoll;
    Cache                            cache;
    Query                            *query;
    bool                             m_isExit;
};

    
#endif

