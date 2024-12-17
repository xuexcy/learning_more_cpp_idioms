#!/bin/bash
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/12/02 22:21:00
# Desc    :
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

source ${CUR_DIR}/../../../env_variable.sh

task_name=expression_template_array_profile_$compiler
bin_dir=$build_output_test_dir
output_dir=${CUR_DIR}/../output/profile
mkdir -p $output_dir
prof_file=$output_dir/$task_name.prof
text_file=$output_dir/$task_name.txt
svg_file=$output_dir/$task_name.svg
compiler_info_file=$output_dir/$task_name.compiler_version.txt
bin_path=$bin_dir/$task_name
$CXX -v > $compiler_info_file 2>&1

#pprof --gv $bin_path $prof_file
#exit 0
env CPUPROFILE=$prof_file $bin_path
pprof $bin_path $prof_file --text > $text_file
pprof $bin_path $prof_file --svg > $svg_file

cd -
