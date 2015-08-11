#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "utils/ThreadPool.h"
#include "utils/IoHandler.hpp"
using namespace std;

#define SERV_PORT 8009





int main()
{
    IoHandler ioHandler;
    ioHandler.ioLoop(10086);
}
































