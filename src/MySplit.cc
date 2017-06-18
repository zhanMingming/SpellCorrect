#include"MySplit.h"

MySplit * MySplit::instance = NULL;
MutexLock MySplit::lock;

MySplit * MySplit::getInstance()
{
    if (NULL ==  instance) {
        lock.lock();
        if (NULL == instance) {
            instance = new MySplit();
        }
        lock.unlock();
    }
    return instance;
}

