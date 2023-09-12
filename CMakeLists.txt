cmake_minimum_required(VERSION 3.26)
project(ZHIHttpd C)
set(CMAKE_C_STANDARD 11)
add_executable(ZHIHttpd
        server.c)
target_link_libraries(ZHIHttpd unp)