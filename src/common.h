#ifndef SPHCACHE_COMMON_H
#define SPHCACHE_COMMON_H
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#define DEBUG(m) \
        printf("%s\n", m); 


#define  NO_THISKEY  "402 key wrong" 
#define  NO_COMMAND  "401 command wrong"
#define  SET_SUCESS  "201 set sucess"

#define KEY 1023
const int MAXCONN = 128;
const int conf_threadNums = 4;
const int conf_size = 102400;
#define  LENOFPTR  sizeof(void*)
#define  LENOFINT  sizeof(int)

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


struct Session
{
    Session()
    :shared(0),size(1),threadNums(1)
    {}

    void *shared;
    size_t size;

    int threadNums;
};
 
static void setNonBlock(int fd)
{
    int stat = fcntl(fd, F_GETFL);
    CHECK((stat==-1),"fcntl_GETFL");
    stat |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, stat);
    CHECK((ret==-1)," fcntl_SETFL");
}
/*
size_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    size_t nread;
    char *bufp = (char*)buf;
    while (nleft > 0) {
        if (( nread == read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR || errno == EAGAIN ) {
                continue;
                return -1;
            } 
        } else if (nread == 0) {
            return nread;
        }
        bufp += nread;
        nleft -= nread;
    }
    return count;
}

size_t wirten(int fd, void* buf, size_t count)
{
    size_t left = count;
    size_t nwritten;
    char *bufp = (char*)buf;
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
                return -1;
            }
        } else  if (nwritten == 0) {
            return nwritten;
        }
        bufp += nwritten;
        nleft += nwritten;
    }
    return count;
}

  */  



 
            
 
#endif

