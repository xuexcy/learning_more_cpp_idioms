cmake_minimum_required(VERSION 3.18)
project(learning_more_cpp_idioms_utils)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")

add_library(common_header INTERFACE)
target_include_directories(common_header INTERFACE main_decorator.h)
set_target_properties(common_header PROPERTIES LINKER_LANGUAGE CXX)

