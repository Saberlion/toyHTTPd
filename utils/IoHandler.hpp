//
// Created by arthur on 15-8-10.
//

#ifndef TOYHTTPD_IOHANDLER_HPP
#define TOYHTTPD_IOHANDLER_HPP

#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include "NonCopyable.hpp"
#include "NetBuffer.hpp"

class IoHandler:public saberUtils::Noncopyable{
#define MAX_EPOLL_FD 4096
public:
    void ioLoop(int serverPort);
    void handle_output(int ep_fd, NetBuffer * clientBuffer);
    void handle_input(int ep_fd, NetBuffer * clientBuffer, const char *rsps_msg_fmt);
    void destroy_fd(int fd, int client_fd, NetBuffer *data_ptr, int case_no);

private:
    const char *rsps_msg_fmt = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\nContent-Type: text/plain\r\n\r\n";

    void setNonblocking(int sock);
};

void IoHandler::destroy_fd(int fd, int client_fd, NetBuffer *data_ptr, int case_no)
{
    struct epoll_event ev;
    ev.data.ptr = data_ptr;
    epoll_ctl(fd, EPOLL_CTL_DEL, client_fd, &ev);
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    delete data_ptr;
}

void IoHandler::handle_input(int ep_fd, NetBuffer * clientBuffer,const char *rsps_msg_fmt)
{
    size_t npos = 0;
    size_t total = 0;
    int ret = 0;
    int case_no = 0;
    char headmsg[256];
    char *sep = NULL;
    const char *CRLF = "\r\n\r\n";
    const char *LF = "\n\n";
    const char *sep_flag=NULL;

    struct epoll_event ev;
    int cfd = clientBuffer->fd;
    size_t pkg_len = 0;

    assert(clientBuffer->in_buf_cur >= 0);
    ret = recv(cfd, clientBuffer->in_buf + clientBuffer->in_buf_cur, 512, MSG_DONTWAIT);
    //printf("%u\n",(unsigned int)pthread_self());
    if (0 == ret || (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        case_no = 1;
        destroy_fd(ep_fd, cfd, clientBuffer, case_no);
        return;
    }

    clientBuffer->in_buf_cur += ret;
    clientBuffer->in_buf[clientBuffer->in_buf_cur] = '\0';

    sep = strstr(clientBuffer->in_buf, CRLF);
    if (NULL == sep) {
        sep = strstr(clientBuffer->in_buf, LF);
        if (NULL == sep)
            return;
        else
            sep_flag = LF;
    } else {
        sep_flag = CRLF;
    }

    if (strstr(clientBuffer->in_buf, "GET ") == clientBuffer->in_buf) {
        if (strstr(clientBuffer->in_buf, "HTTP/1.0") != NULL) {
            clientBuffer->version = HTTP_1_0;
            if (NULL == strstr(clientBuffer->in_buf, "Connection: Keep-Alive")) {
                clientBuffer->keep_alive = 0;
            }
        } else {
            clientBuffer->version = HTTP_1_1;
        }
    }
    npos = strcspn(clientBuffer->in_buf, "\r\n");
    if (npos > 250)
        npos = 250;
    memcpy(headmsg, clientBuffer->in_buf, npos);
    headmsg[npos] = '\0';


    pkg_len = sep - clientBuffer->in_buf + strlen(sep_flag);

    assert(pkg_len >= 0);
    assert(clientBuffer->in_buf_cur - pkg_len >= 0);
    memmove(clientBuffer->in_buf, sep + strlen(sep_flag), clientBuffer->in_buf_cur - pkg_len);
    clientBuffer->in_buf_cur -= pkg_len;

    clientBuffer->out_buf_cur = 0;
    const char* response  = "hello";
    total = snprintf(clientBuffer->out_buf, MAX_OUT_BUF_SIZE, rsps_msg_fmt, strlen(response));
    memcpy(clientBuffer->out_buf + total, response, strlen(response));
    total += strlen(response);

    clientBuffer->out_buf_total = total;

    ev.data.ptr = clientBuffer;
    ev.events = EPOLLOUT;
    epoll_ctl(ep_fd, EPOLL_CTL_MOD, cfd, &ev);
}

void IoHandler::handle_output(int ep_fd, NetBuffer * clientBuffer)
{
    int cfd, ret, case_no;
    struct epoll_event ev;

    cfd = clientBuffer->fd;
    ret = send(cfd, clientBuffer->out_buf + clientBuffer->out_buf_cur, clientBuffer->out_buf_total - clientBuffer->out_buf_cur, MSG_NOSIGNAL);
    if (ret >= 0)
        clientBuffer->out_buf_cur += ret;

    if (0 == ret || (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        //printf("loose 2\n");
        case_no = 2;
        //perror("send");
        //printf("cfd: %d\n", cfd);
        destroy_fd(ep_fd, cfd, clientBuffer, case_no);
        return;
    }
    if (clientBuffer->out_buf_cur == clientBuffer->out_buf_total) {     //have sent all

        //printf("alive: %d\n", clientBuffer->keep_alive);
        if (clientBuffer->version == HTTP_1_0 && 0 == clientBuffer->keep_alive) {
            case_no = 4;
            destroy_fd(ep_fd, cfd, clientBuffer, case_no);
            return;
        }
        ev.data.ptr = clientBuffer;
        ev.events = EPOLLIN;
        epoll_ctl(ep_fd, EPOLL_CTL_MOD, cfd, &ev);
    }
}


void IoHandler::ioLoop(int serverPort) {
    struct epoll_event ev, events[MAX_EPOLL_FD];

    socklen_t clilen;
    int ep_fd = epoll_create(4096);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //把socket设置为非阻塞方式

    setNonblocking(listenfd);
    //设置与要处理的事件相关的文件描述符

    ev.data.fd = listenfd;
    //设置要处理的事件类型

    ev.events=EPOLLIN|EPOLLET;
    //注册epoll事件

    epoll_ctl(ep_fd,EPOLL_CTL_ADD,listenfd,&ev);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port=htons(serverPort);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
        perror("bind error");
        exit(-1);
    }
    if(listen(listenfd, 10))
    {
        perror("listen error");
        exit(-1);
    }

    for(;;)
    {
        int ndfs = epoll_wait(ep_fd,events,MAX_EPOLL_FD,1000);
        for(int i =0;i <ndfs;i++)
        {
            if(events[i].data.fd == listenfd)
            {
                int connfd;
                while(1)
                {
                    connfd= accept(listenfd,(sockaddr*)&clientaddr,&clilen);
                    if(connfd < 0)
                    {
                        break;
                    }
                    if (connfd >= MAX_EPOLL_FD)
                    {
                        close(connfd);
                        continue;
                    }
                    setNonblocking(connfd);
                    NetBuffer * buffer = new NetBuffer();
                    buffer->fd = connfd;
                    buffer->addr = clientaddr;
                    ev.data.ptr = (void *)buffer;
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(ep_fd, EPOLL_CTL_ADD, connfd, &ev);
                }
                continue;
            }

            if(events[i].events&EPOLLIN)
            {
                //std::cout << "handle_input"<< i << std::endl;
                handle_input(ep_fd, (NetBuffer *)events[i].data.ptr,rsps_msg_fmt);

            }
            else if(events[i].events&EPOLLOUT)
            {
                //std::cout << "handle_output"<< i << std::endl;
                handle_output(ep_fd, (NetBuffer *)events[i].data.ptr);

            }
            else if (events[i].events & EPOLLERR) {
                //std::cout << "destroy_fd"<< i << std::endl;
                destroy_fd(ep_fd,((NetBuffer *)events[i].data.ptr)->fd,(NetBuffer *)events[i].data.ptr,3 );
            }
        }

    }
    close(ep_fd);
}

void IoHandler::setNonblocking(int sock)
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
#endif //TOYHTTPD_IOHANDLER_HPP