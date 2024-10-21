cmake_minimum_required(VERSION 3.18)
project(learning_more_cpp_idioms)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")

set(executable_names
address_of
algebraic_hierarchy
attach_by_initialization
)

# https://stackoverflow.com/questions/69517191/cmake-create-executable-for-all-cpp-file-in-folder
foreach (target_name IN LISTS executable_names)
    add_executable(${target_name} ${target_name}.cpp)
endforeach ()

add_subdirectory(${attach_by_initialization}_deps)
target_link_libraries(attach_by_initialization server)
