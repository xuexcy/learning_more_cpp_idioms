if(DEFINED ENV{build_output_bin_dir})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{build_output_bin_dir})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)
endif()
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_subdirectory(utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(main_name address_of)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name algebraic_hierarchy)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name attach_by_initialization)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
add_subdirectory(${main_name}_deps)
target_link_libraries(${main_name} server)

set(main_name attorney_client)
add_executable(${main_name} attorney_client.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name barton_nackman_trick)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name boost_mutant)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
target_link_libraries(${main_name} boost_bimap)

set(main_name calling_virtuals_during_initialization)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name capability_query)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name checked_delete)
file(GLOB main_deps ${main_name}_deps/*.h ${main_name}_deps/*.cpp)
add_executable(${main_name} ${main_name}.cc ${main_deps})
target_link_libraries(${main_name} main_decorator)

set(main_name clear_and_minimize)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name coercion_by_member)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name computational_constructor)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name concrete_data_type)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name construct_on_first_use)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name construction_tracker)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name copy_and_swap)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name copy_on_write)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name counted_body)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name covariant_return_types)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name curiously_recurring_template_pattern)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name empty_base_optimization)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name enable_if)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name erase_remove)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name execute_around_pointer)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name expression_template)
add_subdirectory(${main_name}_deps)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
#target_link_libraries(${main_name} array_compare_calculation)

set(main_name final_class)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name function_poisoning)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name generic_container)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name hierarchy_generation)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name inline_guard_macro)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
# add_definitions(-DENABLE_INLINE)
# add_definitions(-DENABLE_PRINT_HI)
set(deps ${main_name}_deps)
add_subdirectory(${deps})
target_link_libraries(${main_name} ${deps}_test)

set(main_name int_to_type)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name inner_class)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name interface_class)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name iterator_pair)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name making_new_friends)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name member_detector)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name metafunction)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name move_constructor)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(file_name move_constructor)
    set(main_name ${file_name}_no_elide_constructor)
    add_executable(${main_name} ${file_name}.cc)
    target_compile_options(${main_name} PRIVATE -fno-elide-constructors)
    target_link_libraries(${main_name} main_decorator)
endif()

set(main_name multi_statement_macro)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name named_constructor)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name named_parameter)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name named_loop)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name nifty_counter)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
set(deps ${main_name}_deps)
add_subdirectory(${deps})
target_link_libraries(${main_name} ${deps}_a ${deps}_b ${deps}_stream)

set(main_name non_copyable_mixin)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name non_throwing_swap)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name nullptr)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name non_virtual_interface)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name parameterized_base)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name pointer_to_implementation)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)
add_subdirectory(${main_name}_deps)
target_link_libraries(${main_name} book book_with_pimpl book_template_with_pimpl)

set(main_name policy_clone)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name polymorphic_exception)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name resource_acquisition_is_initialization)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name resource_return)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name return_type_resolver)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name safe_bool)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name SFINAE)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name tag_dispatching)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

set(main_name type_generator)
add_executable(${main_name} ${main_name}.cc)
target_link_libraries(${main_name} main_decorator)

