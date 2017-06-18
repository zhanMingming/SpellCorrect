#ifndef SPHCACHE_SPHBUFFER_H
#define SPHCACHE_SPHBUFFER_H

#include"MutexLock.h"
#include"Condition.h"
#include<queue>


template<class T>
class Buffer
{
public:
    Buffer(size_t size);
    ~Buffer(){}
    void push(const T & task);
    T pop();

    bool empty();
    bool full();
private:
    MutexLock   m_mutex;
    Condition   m_notFull;
    Condition   m_notEmpty;
    size_t         m_size;
    std::queue<T>  m_que;
};

#include"Buffer.cc"
#endif

