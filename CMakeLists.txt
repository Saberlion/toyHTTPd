cmake_minimum_required(VERSION 3.2)
project(toyHTTPd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES
    main.cpp
    utils/NetBuffer.hpp
    utils/IoHandler.hpp
    utils/NonCopyable.hpp
    utils/ThreadPool.h)
add_executable(toyHTTPd ${SOURCE_FILES})