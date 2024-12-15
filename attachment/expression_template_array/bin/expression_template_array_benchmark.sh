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

run() {
  task_name=expression_template_array_benchmark_$compiler
  if [[ $1 == 2 ]]; then
    task_name=expression_template_array_benchmark_2_$compiler
  fi
  bin_dir=$expression_template_array_attachment_bin_dir
  output_dir=$expression_template_array_attachment_dir/output
  mkdir -p $output_dir
  bin_path=$bin_dir/$task_name

  $bin_path > $output_dir/$task_name.txt
}
run
run 2
cd -
