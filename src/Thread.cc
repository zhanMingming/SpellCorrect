#include"Thread.h"

Thread::Thread()
:m_pthId(0),
m_isRunning(false)
{}

void Thread::start()
{
    pthread_create(&m_pthId, NULL, runInThread,this);
    m_isRunning = true;
}

void Thread::join()
{
    pthread_detach(m_pthId);
    m_isRunning = false;
}

void * Thread::runInThread(void *arg)
{
    Thread *pThread =static_cast<Thread*>(arg);
    pThread->run();
    return NULL;
}

Thread::~Thread()
{
    if (m_isRunning) {
        pthread_detach(m_pthId);
        m_isRunning = false;
    }
}


