cmake_minimum_required(VERSION 3.30)
project(http_keylogger C)

set(CMAKE_C_STANDARD 23)

add_executable(http_keylogger
        main.c
        #winapi_test.c
)
