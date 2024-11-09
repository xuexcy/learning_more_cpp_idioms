cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(book book.h book.cpp)
add_library(book_with_pimpl book_with_pimpl.h book_with_pimpl.cpp)
add_library(book_template_with_pimpl book_template_with_pimpl.h book_template_with_pimpl.cpp)
