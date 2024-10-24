cmake_minimum_required(VERSION 3.18)
project(learning_more_cpp_idioms)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")

set(executable_names
address_of
algebraic_hierarchy
attach_by_initialization
attorney_client
barton_nackman_trick
curiously_recurring_template_pattern
)

add_subdirectory(src/utils)
# https://stackoverflow.com/questions/69517191/cmake-create-executable-for-all-cpp-file-in-folder
foreach (target_name IN LISTS executable_names)
    add_executable(${target_name} src/${target_name}.cpp)
    target_link_libraries(${target_name} common_header)
endforeach ()

add_subdirectory(src/attach_by_initialization_deps)
target_link_libraries(attach_by_initialization server)
