cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(message_queue message_queue.h message_queue.cpp)
