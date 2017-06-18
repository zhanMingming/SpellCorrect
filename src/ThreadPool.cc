#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <ctype.h>
#include <vector>
#include <string.h>
#include "common.h"
#include "MyThread.h"
#include "ThreadPool.h"
#include "Query.h"

void strTrimCrlf(char *str)
{
    char *p = &str[strlen(str)-1];
    while (*p == '\r' || *p == '\n') {
        *p-- = '\0';
    }
}

void str_split(const char *str, char *cmd, char *rest, char c)
{
    const char *p = strchr(str, c); 
    if (p == NULL) {
        strcpy(cmd, str);
    } else {
        strncpy(cmd, str, p - str);
        strcpy(rest, p + 1);
    }
}

void str_upper(char *str)
{
    while (*str) {
        *str = toupper(*str);
        ++str;
    }
}



size_t readn(int fd, void *buf, size_t count)
{
//    std::cout << "start to readi :" << count << std::endl;
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char*)buf;
    while (nleft > 0) {
        if (( nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR || errno == EAGAIN ) {
                continue;
                return -1;
            } 
        } else if (nread == 0) {
            return nread;
        }
  //      sleep(1);
  //      std::cout << " have read it:" << nread << std::endl;
        bufp += nread;
        nleft -= nread;
    }
    return count;
}

size_t writen(int fd, void* buf, size_t count)
{
   // std::cout << "start to write" << std::endl;
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char*)buf;
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
                return -1;
            }
        } else if (nwritten == 0) {
            return nwritten;
        }
     //   std::cout << "have write it :" << nwritten << std::endl;
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}


struct Message
{
    int m_length;
    char m_text[1024 * 20];
};

ThreadPool::ThreadPool(Epoller &epoll)
:m_queFd(INT_MAX),
m_epoll(epoll),
query(Query::getInstance()),
m_isExit(false)
{
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::add(int fd)
{
    printf("I'm the  main thread, put fd into  queue, My id = %lld\n", pthread_self());
    m_queFd.push(fd);
}

void ThreadPool::start()
{
    const int nums = 4;  
    for (int index = 0; index != nums; ++index) 
    {
        Thread *pthread = new MyThread(*this, ThreadType::WorkerThread);
        pthread->start();
        m_vecThreads.push_back(pthread);
    }
}

void ThreadPool::stop()
{
    if (!m_isExit) {
        m_isExit = true;
        std::vector<Thread*>::iterator iter;
        for (iter = m_vecThreads.begin(); iter !=m_vecThreads.end();++iter)
        {
            (*iter)->join();
        }
    }
}

void ThreadPool::doWorker()
{

    printf("I'm a Worker thread, id = %lld\n", pthread_self());
    while(1) {        
        int fd = m_queFd.pop();
        printf("I'm a Worker thread, id = %lld", pthread_self());
        printf(" And  i get fd is  %d\n", fd);
        while (1) {
            struct  Message recvMessage;
            struct  Message sendMessage;
            memset(&recvMessage, 0, sizeof(struct Message));
            memset(&sendMessage, 0, sizeof(struct Message));
            int readCount = 0;
            std::cout << "read" << std::endl;
            readCount = readn(fd, &(recvMessage.m_length), LENOFINT);
            CHECK(readCount == -1, "readn");
            if (readCount == 0) {
                perror("connect close");
                close(fd);
                break;
            }
            readCount = readn(fd, &(recvMessage.m_text), recvMessage.m_length);
            CHECK(readCount == -1, "readn");
            if (readCount == 0) {
                perror("connect close");
                close(fd);
                break;
            }
            strTrimCrlf(recvMessage.m_text);
            std::vector<std::pair<std::string, int>> vec = query->getSimilarWords(recvMessage.m_text, cache);
            std::string ret = " ";
            if (!vec.empty()) {
                std::vector<std::pair<std::string, int>>::iterator iter = vec.begin();
                for (;iter != vec.end(); ++iter) {
                    std::cout << iter->first << " " << iter->second << " ";
                }
                ret = vec[0].first;
            } 
            
            sendMessage.m_length = strlen(ret.c_str());
            strncpy(sendMessage.m_text, ret.c_str(), sendMessage.m_length);
            int ret1 = writen(fd, &sendMessage, sendMessage.m_length + LENOFINT);
            CHECK(ret1 == -1, "writen");

/*
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            m_epoll.add(fd, ev);
            break;            
*/
        }
    }
}

void ThreadPool::doSync()
{
    while(1) {     
        printf("I'm a doSync thread, id = %lld\n", pthread_self());
    }
}



     
 
