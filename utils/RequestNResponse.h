//
// Created by arthur on 15-8-13.
//

#ifndef TOYHTTPD_REQUEST_H
#define TOYHTTPD_REQUEST_H

#include <unordered_map>
#include "NetCoding.hpp"
class RequstHanler{
public:
    //to do
    void parse(const string& orig)
    {
        rawData_ = orig;

    }
    void reset()
    {
        method_.clear();
        version.clear();
        header_.clear();
        body_.clear();
    }


    string method_;
    string version_;
    std::unorder_map<string, string>header_;
    string body_;

    string rawData_;
private:



};

class ResponseHandler{
public:
    ResponseHandler();
    ResponseHandler(int stat_code,
                    std::unordered_map<string, string>header,
                    string content_type,
                    size_t length,
                    string body)
            :stat_code_(stat_code),
             header_(header),
             content_type_(content_type),
             length_(length),
             body_(body){}

    int stat_code_;
    string status_;
    std::unordered_map<string,string>header_;
    string content_type_;
    size_t length_;
    string body_;

    string rawData_;
private:
    const char* const HTTP_RESPONSE = "HTTP/1.1 %d %s\r\nConnection: %s\r\nServer: toyHTTPd/1.0.0\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length: %d\r\n\r\n%s";

};
#endif //TOYHTTPD_REQUEST_H
