
template<class T>
Buffer<T>::Buffer(size_t size)
:m_size(size),
m_mutex(),
m_notFull(&m_mutex),
m_notEmpty(&m_mutex)
{}

template<class T>
bool Buffer<T>::empty()
{
    return m_que.size() == 0;
}

template<class T>
bool Buffer<T>::full()
{
    return m_que.size() == m_size;
}

template<class T>
void Buffer<T>::push(const T &task)
{
    m_mutex.lock();
    while(full()) {
        m_notFull.wait();
    }
    m_que.push(task);
    m_mutex.unlock();
    m_notEmpty.notify();
}

template<class T>
T Buffer<T>::pop()
{
    m_mutex.lock();
    while(empty()) {
        m_notEmpty.wait();
    }
    T task = m_que.front();
    m_que.pop();
    m_mutex.unlock();
    m_notFull.notify();
    return task;
}

