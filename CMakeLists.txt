cmake_minimum_required(VERSION 3.28)
project(learning_more_cpp_idioms)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
message("thirdparty_dir: " ${THIRDPARTY_DIR})

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# import cpp_utils
FetchContent_Declare(
    cpp_utils
    GIT_REPOSITORY https://github.com/xuexcy/cpp_utils.git
    GIT_TAG main
    SOURCE_DIR ${THIRDPARTY_DIR}/cpp_utils
)
FetchContent_MakeAvailable(cpp_utils)
include_directories(${THIRDPARTY_DIR}/cpp_utils)
set(EXTRA_LIBS ${EXTRA_LIBS} cpp_utils)


# import boost
set(BOOST_INCLUDE_LIBRARIES bimap)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
    boost
    # GIT_REPOSITORY https://github.com/boostorg/boost.git
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    SOURCE_DIR ${THIRDPARTY_DIR}/boost
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(boost)
#add_subdirectory(${THIRDPARTY_DIR}/boost EXCLUDE_FROM_ALL)
# set(libs bimap)
# foreach(lib IN LISTS libs)
#     include_directories(${THIRDPARTY_DIR}/boost/libs/${lib}/include)
# endforeach()


# set(executable_names
# address_of
# algebraic_hierarchy
# attach_by_initialization
# attorney_client
# barton_nackman_trick
# base_from_member
# curiously_recurring_template_pattern
# boost_mutant
# )

add_subdirectory(src/utils)
# https://stackoverflow.com/questions/69517191/cmake-create-executable-for-all-cpp-file-in-folder
# foreach (target_name IN LISTS executable_names)
#     add_executable(${target_name} src/${target_name}.cc)
#     target_link_libraries(${target_name} common_header)
#     target_link_libraries(${target_name} ${EXTRA_LIBS})
# endforeach ()









set(main_name address_of)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)

set(main_name algebraic_hierarchy)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)

set(main_name attach_by_initialization)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)
add_subdirectory(src/${main_name}_deps)
target_link_libraries(${main_name} server)

set(main_name attorney_client)
add_executable(${main_name} src/attorney_client.cc)
target_link_libraries(${main_name} common_header)

set(main_name barton_nackman_trick)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)

set(main_name boost_mutant)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)
target_link_libraries(${main_name} boost_bimap)

set(main_name curiously_recurring_template_pattern)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)

set(main_name calling_virtuals_during_initialization)
add_executable(${main_name} src/${main_name}.cc)
target_link_libraries(${main_name} common_header)
