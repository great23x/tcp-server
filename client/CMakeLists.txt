cmake_minimum_required(VERSION 3.27)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client client.c)
target_link_libraries(client ws2_32)
