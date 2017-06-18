#ifndef CLIENT_H
#define CLIENT_H

#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#define DEBUG(m) \
        printf("%s\n", m);
 
#define  LENOFINT  sizeof(int)
#define  LENOFPTR  sizeof(void*)

#define CHECK(NUM1, str) \
    if (NUM1) { \
        ERR_EXIT((str)); \
    }

#define ERR_EXIT(m) \
    do \
    {  \
        perror(m); \
        exit(EXIT_FAILURE);\
    }while(0)

struct Message
{
    int m_length;
    char m_text[1024 * 20];
};

 
static void setNonBlock(int fd)
{
    int stat = fcntl(fd, F_GETFL);
    CHECK((stat==-1),"fcntl_GETFL");
    stat |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, stat);
    CHECK((ret==-1)," fcntl_SETFL");
}

size_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    size_t nread;
    char *bufp = (char*)buf;
    std::cout << "start to read" << std::endl;
    while (nleft > 0) {
        if (( nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR || errno == EAGAIN ) {
                continue;
                return -1;
            } 
        } else if (nread == 0) {
            return nread;
        }
        std::cout << "readn:" << nread << std::endl;
        bufp += nread;
        nleft -= nread;
    }
    return count;
}

size_t writen(int fd, void* buf, size_t count)
{
    size_t nleft = count;
    size_t nwritten;
    char *bufp = (char*)buf;
    std::cout << "start to write" << std::endl;
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
                return -1;
            }
        } else  if (nwritten == 0) {
            return nwritten;
        }
        std::cout << "writen:" << nwritten << std::endl;
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}
 
#endif
