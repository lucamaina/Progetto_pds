cmake_minimum_required(VERSION 3.13)
project(lab_3)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab_3 Message.cpp Message.h NetworkServer.cpp NetworkServer.h SharedEditor.cpp SharedEditor.h Symbol.cpp Symbol.h main.cpp)