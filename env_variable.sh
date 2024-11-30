#!/bin/bash
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

# cmake 工作目录
export build_dir=${CUR_DIR}/build

# 项目产出目录
export build_output_dir=$build_dir/output
# 编译产出的可执行文件存放目录
export build_output_bin_dir=$build_output_dir/bin

export default_execute_file=expression_template_array_test

# 特殊目录
## parameterized_base 在 rust 语言上的一些实现
export parameterized_base_rust=src/parameterized_base_deps/rust_traits_serializable
## inner_class 在 rust 语言上的一些实现
export inner_class_rust=src/inner_class_deps/rust_traits_same_function_name

