cmake_minimum_required(VERSION 3.13)
project(cs109pa2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(cs109pa2
        commands.cpp
        commands.h
        debug.cpp
        debug.h
        file_sys.cpp
        file_sys.h
        main.cpp
        util.cpp
        util.h)
