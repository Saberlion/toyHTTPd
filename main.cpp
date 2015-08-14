#include <iostream>
#include <string.h>
#include "utils/ThreadPool.hpp"
#include "utils/IoHandler.hpp"
#include "utils/RqsNRsp.hpp"
#include <fstream>
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



}
































