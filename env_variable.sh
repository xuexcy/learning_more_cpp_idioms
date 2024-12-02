#!/bin/bash
CUR_DIR=$(cd `dirname $0`; pwd)

if [ "${BASH_SOURCE[0]}" != "" ];
then
    CUR_DIR=$(cd `dirname ${BASH_SOURCE[0]}`; pwd)
fi

echo "Project Root Dir:" $CUR_DIR

export compiler=clang++
#export compiler=g++

# cmake 工作目录
if [[ $compiler == "g++" ]];
then
    export CC="gcc"
    export CXX="g++"
    export build_dir=${CUR_DIR}/build_g++
else
    export CC="clang"
    export CXX="clang++"
    export build_dir=${CUR_DIR}/build
fi
echo "Compiler:" $CXX
$CXX -v

# 项目产出目录
export build_output_dir=$build_dir/output
# 编译产出的可执行文件存放目录
export build_output_bin_dir=$build_output_dir/bin
# 附件目录
export attachment_dir=${CUR_DIR}/attachment

export default_execute_file=expression_template_array_profile

# 特殊目录
## parameterized_base 在 rust 语言上的一些实现
export parameterized_base_rust=src/parameterized_base_deps/rust_traits_serializable
## inner_class 在 rust 语言上的一些实现
export inner_class_rust=src/inner_class_deps/rust_traits_same_function_name

