#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "utils/ThreadPool.h"
using namespace std;

#define SERV_PORT 8009


void setNonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

void * task(int fd)
{
    unsigned int n;
    char recvBuf[1024] = {0};
    int ret = 999;
    int rs = 1;

    while (rs)
    {
        ret = recv(fd, recvBuf, 1024, 0);// 接受客户端消息

        if (ret < 0)
        {
            //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可//读在这里就当作是该次事件已处理过。

            if (errno == EAGAIN)
            {
                printf("EAGAIN\n");
                break;
            }
            else
            {
                printf("recv error!\n");

                close(fd);
                break;
            }
        }
        else if (ret == 0) {
            // 这里表示对端的socket已正常关闭.

            rs = 0;
        }
        if (ret == sizeof(recvBuf))
            rs = 1; // 需要再次读取

        else
            rs = 0;

        if (ret > 0) {

            char buf[1000] = {0};
            sprintf(buf, "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n%s", "Hello world!\n");
            cout<<buf;
            send(fd, buf, strlen(buf), 0);
            close(fd);

        }
    }

}
int main() {
    struct epoll_event ev, events[256];
    int epfd;
    socklen_t clilen;
    epfd=epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //把socket设置为非阻塞方式

    setNonblocking(listenfd);
    //设置与要处理的事件相关的文件描述符

    ev.data.fd=listenfd;
    //设置要处理的事件类型

    ev.events=EPOLLIN|EPOLLET;
    //注册epoll事件

    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, 10);
    ThreadPool threadPool(4);
    for(;;)
    {
        int ndfs = epoll_wait(epfd,events,256,6);
        for(int i =0;i <ndfs;i++)
        {
            if(events[i].data.fd == listenfd)
            {
                int connfd = accept(listenfd,(sockaddr*)&clientaddr,&clilen);
                if(connfd < 0)
                {
                    perror("connfd < 0");
                    exit(1);
                }
                setNonblocking(connfd);
                char *str = inet_ntoa(clientaddr.sin_addr);
                ev.data.fd =connfd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            if(events[i].events&EPOLLIN)
            {
                int sockfd = events[i].data.fd;
                if(sockfd <0)continue;
                threadPool.enqueue(task, sockfd);

            }
            else if(events[i].events&EPOLLOUT)
            {
                int sockfd = events[i].data.fd;
                char buf[1000];
                sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World", 11);
                int nwrite, data_size = strlen(buf);
                int n = data_size;
                while (n > 0) {
                    nwrite = write(sockfd, buf + data_size - n, n);
                    if (nwrite < n) {
                        if (nwrite == -1 && errno != EAGAIN) {
                            perror("write error");
                        }
                        break;
                    }
                    n -= nwrite;
                }
                close(sockfd);
                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);

            }
        }

    }
    close(epfd);
}































