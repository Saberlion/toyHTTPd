//
// Created by arthur on 15-8-10.
//

#ifndef TOYHTTPD_BUFFER_HPP
#define TOYHTTPD_BUFFER_HPP

#include <sys/socket.h>
enum version_t {
    HTTP_1_0 = 10,
    HTTP_1_1 = 11
};
#define MAX_IN_BUF_SIZE (1<<14)
#define MAX_OUT_BUF_SIZE (1<<20)
class NetBuffer {
public:
    int fd;
    struct sockaddr_in addr;
    char *in_buf;
    char *out_buf;
    int in_buf_cur = 0;
    int out_buf_cur = 0;
    int out_buf_total;
    int keep_alive = 1;
    enum version_t version;

    NetBuffer()
    {
        in_buf = new char[MAX_IN_BUF_SIZE];
        out_buf = new char[MAX_OUT_BUF_SIZE];
    }
    ~NetBuffer()
    {
        delete[] in_buf;
        delete[] out_buf;
    }
};
#endif //TOYHTTPD_BUFFER_HPP
