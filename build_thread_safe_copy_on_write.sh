#!/bin/bash
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/15 19:22:05
# Desc    :   build src/thread_safe_copy_on_write.cc by g++ because clang++ doesn't support std::atomic<std::shared_ptr>
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

source ./env_variable.sh

name=thread_safe_copy_on_write
# I use gcc 14.2.0_1
g++ src/$name.cc -std=c++23 -o $build_output_bin_dir/$name









cd -



