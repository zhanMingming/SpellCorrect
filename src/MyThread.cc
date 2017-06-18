#include"MyThread.h"
#include"ThreadPool.h"


MyThread::MyThread(ThreadPool &threadpool, int type)
:m_threadpool(threadpool),
m_type(type)
{}

void MyThread::run()
{
    switch(m_type) {
        case ThreadType::WorkerThread:
        {
            m_threadpool.doWorker();
            break;
        }
        case ThreadType::SyncThread:
        {
            m_threadpool.doSync();
            break;
        }
    }
}
 
