cmake_minimum_required(VERSION 3.30)
project(http_keylogger C)

set(CMAKE_C_STANDARD 23)

add_executable(http_keylogger
        #main.c
        winapi_test.c
)
add_executable(http_server_test
        http_server_test.c
)
target_link_libraries(http_server_test PRIVATE ws2_32)

add_executable(http_client_test
        http_client_test.c
)
target_link_libraries(http_client_test PRIVATE ws2_32)