#include <iostream>
#include <string.h>
#include "utils/ThreadPool.hpp"
#include "utils/IoHandler.hpp"
#include "utils/RqsNRsp.hpp"
#include <fstream>
#include "utils/Cache.hpp"
using namespace std;
int main()
{

    std::fstream fr;
    fr.open("/home/arthur/ClionProjects/toyHTTPd/test/requestheader",std::ios::in | std::ios::binary);
    fr.seekg(0,std::ios::end);
    int len = fr.tellg();
    fr.seekg (0, std::ios::beg);
    char * buffer = new char[len];
    fr.read(buffer,len);
    std::string str(buffer);
    delete[] buffer;
    RequestHandler rh;
    rh.parse(str);
    fr.close();

    LruCache<int,int> cache(10);
    for(int i =0;i < 10 ;i++)
    {
        cache.set(i,i);
    }

    cache.get(2);
    cache.get(5);
    cache.get(7);

    LfuCache<int, int> lfuCache(10);
    for(int i =0;i < 20 ;i++)
    {
        lfuCache.set(i,i);
    }

    return 0;
}
































