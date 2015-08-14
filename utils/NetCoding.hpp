//
// Created by arthur on 15-8-13.
//

#ifndef TOYHTTPD_NET_HPP
#define TOYHTTPD_NET_HPP



#include <algorithm>
#include <string>

#define BYTE unsigned char
inline BYTE toHex(BYTE x) {
    return x > 9 ? x - 10 + 'A' : x + '0';
}

inline BYTE fromHex(BYTE x) {
    if (isdigit(x)) return x - '0';
    return isupper(x) ? x - 'A' + 10 : x - 'a' + 10;
}


inline std::string URIEncode(const std::string& urlIn) {
    std::string urlOut;
    for (size_t i = 0; i < urlIn.size(); i++) {
        BYTE buf[4];
        memset(buf, 0, sizeof(buf));

        if (isalnum((BYTE) urlIn[i])
            || (urlIn[i] == '-')
            || (urlIn[i] == '_')
            || (urlIn[i] == '.')
            || (urlIn[i] == '~')) {
            buf[0] = urlIn[i];
        } else if (urlIn[i] == ' ') {
            buf[0] = '+';
        } else {
            buf[0] = '%';
            buf[1] = toHex(((BYTE) urlIn[i]) >> 4);
            buf[2] = toHex(((BYTE) urlIn[i]) & 15);
        }
        urlOut += ((char*) buf);
    }
    return urlOut;
}

inline std::string URIDecode(const std::string& urlIn) {
    std::string urlOut;
    for (size_t i = 0; i < urlIn.size(); i++) {
        BYTE ch = 0;

        if (urlIn[i] == '%') {
            ch = (fromHex(urlIn[i + 1]) << 4) | (fromHex(urlIn[i + 2]));
            i += 2;
        } else if (urlIn[i] == '+') {
            ch = ' ';
        } else {
            ch = urlIn[i];
        }

        urlOut += ((char) ch);
    }
    return urlOut;
}

inline std::string upper(const std::string& src) {
    std::string ret = src;
    for_each(ret.begin(), ret.end(), [](char& c) { c = toupper(c); });
    return ret;
}

/*return value optimization will be applied*/
inline std::vector<std::string> splitBySingle(const std::string& src, const std::string& pattern, size_t limit = 0) {
    std::vector<std::string> res;
    if (src.empty()) return res;

    size_t start = 0;
    size_t end = 0;

    while (start < src.size() && (limit == 0 || res.size() < limit)) {
        end = src.find_first_of(pattern, start);
        if (std::string::npos == end) {
            res.push_back(src.substr(start));
            return res;
        }

        res.push_back(src.substr(start, end - start));

        start = end + 1;
    }
    return res;
}

/*return value optimization will be applied*/
inline std::vector<std::string> splitByMulti(const std::string& src, const std::string& pattern, size_t limit = 0) {
    std::vector<std::string> res;
    if (src.empty()) return res;

    size_t start = 0;
    size_t end = 0;

    while (start < src.size() && (limit == 0 || res.size() < limit)) {
        end = src.find(pattern, start);
        if (std::string::npos == end) {
            res.push_back(src.substr(start));
            return res;
        }

        res.push_back(src.substr(start, end - start));

        start = end + pattern.length();
    }
    return res;
}

std::string getHttpStatus(int code)
{
    std::string res;
    switch (code)
    {
        case 200:res="OK";break;
        case 204:res="NO CONTENT";break;
        case 206:res="PARTIAL CONTENT";break;
        case 301:res="MOVE PERMANENTLY";break;
        case 302:res="MOVE TEMPORARILY";break;
        case 304:res="NOT MODIFIED";break;
        case 400:res="BAD REQUEST";break;
        case 401:res="UNAUTHORIZED";break;
        case 403:res="FORBIDDEN";break;
        case 404:res="NOT FOUND";break;
        case 500:res="INTERNAL SERVER ERROR";break;
        case 502:res="BAD GATEWAY";break;
        case 503:res="SERVICE UNAVAILABLE";break;
    }
    return res;
}
#endif //TOYHTTPD_NET_HPP
