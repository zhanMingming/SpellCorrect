#include<iostream>
#include<string.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/shm.h>
#include"common.h"
#include"ThreadPool.h"
#include"Epoller.h"
void do_child();
int tcp_server(const char*, unsigned short port);

void forkChildProcess(int sig); 
int main(void)
{
    pid_t pid = fork();
    if (0 == pid) {
        do_child();
    } else {
        while(true) {
            assert(signal(SIGCHLD, forkChildProcess) != SIG_ERR);
            pause();
        }
    }
}

void forkChildProcess(int sig) 
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    CHECK((pid == -1), "Waitpid");
    if (WIFSIGNALED(status)) {
        int signalNum = WTERMSIG(status);
        std::cout << "ChildProcess was killed by signal num:" << signalNum  << std::endl;
    }
    pid = fork();
    if (0 == pid) {
        std::cout << "New childProcess Forked" << std::endl;
        do_child();
    }
}

void do_child()
{
    Epoller epoll;
    ThreadPool  threadpool(epoll);
    threadpool.start();   // start threadpool
    int port = 8888;
    int listenFd = tcp_server(NULL, port);
    setNonBlock(listenFd);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenFd;
    epoll.add(listenFd, ev);
    while(1) {
        int num = epoll.wait();
        for (int index = 0; index < num; ++index) {
            int fd = epoll.get(index).data.fd;
            if (fd == listenFd) {
                while(1) {
                    struct sockaddr_in client;
                    int len = sizeof(struct sockaddr);
                    int sock = accept(listenFd, (struct sockaddr*)&client, (socklen_t*)(&len));
                    if (sock == -1) {
                        if (errno == EAGAIN || errno == EINTR) {
                            break;
                        } else {
                            ERR_EXIT("accept");
                        }
                    }
                    std::cout << inet_ntoa(client.sin_addr) <<  " connect " << std::endl;
                    setNonBlock(sock);
                    ev.events = EPOLLIN | EPOLLET |EPOLLONESHOT;
                    ev.data.fd = sock;
                    epoll.add(sock, ev);
                }
            } else {
                threadpool.add(fd);
            }
        }
    }
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
    ret = listen(listenFd, MAXCONN);
    CHECK((ret == -1), "listen");
    return listenFd;
}
 
