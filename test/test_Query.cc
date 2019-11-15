#include"Query.h"
#include"Cache.h"
#include<vector>
#include<string>
#include<string.h>
#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ipc.h>


using namespace std;

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
        break;\
    }while(0)

struct Message
{
    int m_length;
    char m_text[1024 * 20];
};

 

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
 

int tcp_server(const char *host, unsigned short port)
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK((listenFd == -1), "socket");     
    struct sockaddr_in  servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); 
    if ( host != NULL) {
        if (0 == inet_aton(host, &servaddr.sin_addr)) {
            struct hostent *hp;
            hp = gethostbyname(host);
            CHECK((hp == NULL), "gethostbyname");
            servaddr.sin_addr = *(struct in_addr*)hp->h_addr;
        }
    } else {
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    } 
    
    servaddr.sin_port = htons(port);
    int on = 1;
    int ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    CHECK((ret == -1), "setsockopt");
    ret = bind(listenFd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr));
    CHECK((ret == -1), "bind");
    ret = listen(listenFd, 10);
    CHECK((ret == -1), "listen");
    return listenFd;
}
 
std::string getWord(std::string data, Cache &cache) {

    Query *q = Query::getInstance();
    std::vector<std::pair<std::string,int>> vec = q->getSimilarWords(data, cache);
    if (!vec.empty()) {
        return vec[0].first;
    } else {
        return "";
    }

}

int main(int argc, char **argv)
{
    Cache cache;
    int listenfd = tcp_server(NULL, 8888);
    while (1) {
 
        int sock = accept(listenfd, NULL, NULL);
        if (sock == -1) {
            if (errno == EAGAIN || errno == EINTR) {
            } else {
                ERR_EXIT("accept");
            }
        }
        while(1) {
            struct Message recvMessage;
            struct Message sendMessage;
            memset(&recvMessage, 0, sizeof(recvMessage));
            memset(&sendMessage, 0, sizeof(sendMessage));
            int readCount = 0;
            readCount = readn(sock, &(recvMessage.m_length), LENOFINT);
            if ( readCount == -1) {
                std::cout << "readCount " << " == -1" << std::endl;
            }
            CHECK(readCount == -1, "readn");
            readCount = readn(sock, &(recvMessage.m_text), recvMessage.m_length);
            CHECK(readCount == -1, "readn");
            std::string ret = getWord(recvMessage.m_text, cache);
            sendMessage.m_length = strlen(ret.c_str());
            strncpy(sendMessage.m_text, ret.c_str(), sendMessage.m_length);
            int r = writen(sock, &sendMessage, sendMessage.m_length + LENOFINT);
            CHECK( r == -1, "writen");
        }
    }
}

