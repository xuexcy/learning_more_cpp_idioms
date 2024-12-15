add_executable(expression_template_array_test calculation_test.cc)
target_link_libraries(expression_template_array_test gtest)

message("expression_template_array_attachment_bin_dir: $ENV{expression_template_array_attachment_bin_dir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{expression_template_array_attachment_bin_dir})

set(main_name expression_template_array_benchmark_$ENV{compiler})
add_executable(${main_name} calculation_benchmark.cc)
target_link_libraries(${main_name} benchmark::benchmark )

set(main_name expression_template_array_benchmark_2_$ENV{compiler})
add_executable(${main_name} calculation_benchmark_2.cc)
target_link_libraries(${main_name} benchmark::benchmark )

set(main_name expression_template_array_profile_$ENV{compiler})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${LIB_PROFILER_DIR} -lprofiler -ltcmalloc")
add_executable(${main_name} calculation_profile.cc)
#target_link_directories(expression_template_array_profile PRIVATE /opt/homebrew/Cellar/gperftools/2.16/lib)
#target_link_libraries(expression_template_array_profile profiler)
