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

private:
    void reset()
    {
        method_.clear();
        version.clear();
        header_.clear();
        body_.clear();
    }
    string rawData_;
    string method_;
    string version_;
    std::unorder_map<string, string>header_;
    string body_;

};

class ResponseHandler{
public:
private:
    int stat_code_;
    std::unordered_map<string,string>header_;
    string body_;
    size_t length_;
};
#endif //TOYHTTPD_REQUEST_H
