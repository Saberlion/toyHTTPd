//
// Created by arthur on 15-8-14.
//

#ifndef TOYHTTPD_RQSNRSP_HPP
#define TOYHTTPD_RQSNRSP_HPP

#include <unordered_map>
#include "NetCoding.hpp"
#include <string>
#include <sstream>

class RequestHandler {
public:
    RequestHandler()=default;
    RequestHandler(std::string rawData):rawData_(rawData){}

    void parse(const std::string& rawData)
    {
        rawData_ = rawData;
        auto res = splitByMulti(rawData_,"\r\n",rawData_.size());
        std::string request_line;
        if(res.size() != 0)
            request_line = res.at(0);
        else
            return;

        auto request_line_split = splitByMulti(request_line," ");
        if(request_line_split.size() == 3)
        {
            method_ = request_line_split.at(0);
            request_url_ = request_line_split.at(1);
            version_ = request_line_split.at(2);

        }
        else
            return;

        auto body_segmet = find(res.begin(),res.end(),"");
        if(body_segmet != res.end())
        {
            for(auto it = res.begin()+1;it != body_segmet;it++)
            {
                auto header_map = splitByMulti(*it,":");
                if(header_map.size()==2)
                    header_[header_map.at(0)] = header_map.at(1);
            }
            body_ = res.back();

        }

    }
    void reset()
    {
        method_.clear();
        version_.clear();
        header_.clear();
        body_.clear();
    }


    std::string method_;
    std::string request_url_;
    std::string version_;
    std::unordered_map<std::string, std::string>header_;
    std::string body_;
    std::string rawData_;
private:



};

class ResponseHandler{
public:
    ResponseHandler();
    ResponseHandler(int stat_code,
                    std::unordered_map<std::string, std::string>header,
                    std::string content_type,
                    std::string body)
            :stat_code_(stat_code),
             header_(header),
             content_type_(content_type),
             body_(body){

        header_["Server"] = "toyHTTPd/pre-alpha";
        header_["Date"] = getTime("%a,%d %b %Y %X");
    }


    void toString()
    {
        char buffer[4096]={0};
        char statusBuffer[30] = {0};
        std:: string header;
        sprintf(statusBuffer,"HTTP/1.1 %d %s",stat_code_, getHttpStatus(stat_code_).c_str());
        header_["Content-Type"] = content_type_;
        header.reserve(1024);
        for(auto&h:header_)
        {
            header+=h.first+":"+h.second+"\r\n";
        }
        sprintf(buffer,HTTP_RESPONSE, statusBuffer,header.c_str(),body_.c_str());

        rawData_ = buffer;


    }
    int stat_code_;
    std::unordered_map<std::string,std::string>header_;
    std::string content_type_;
    std::string body_;
    std::string rawData_;
private:
    const char* const HTTP_RESPONSE = "%s\r\n%s\r\n%s";

};

#endif //TOYHTTPD_RQSNRSP_HPP
