cmake_minimum_required(VERSION 2.8)
project(toyHTTPd)
SET (CMAKE_CXX_COMPILER    "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES
    main.cpp
    utils/NetBuffer.hpp
    utils/IoHandler.hpp
    utils/NonCopyable.hpp
    utils/ThreadPool.hpp
    utils/NetCoding.hpp
    utils/RqsNRsp.hpp
    utils/Cache.hpp)

add_executable(toyHTTPd ${SOURCE_FILES})