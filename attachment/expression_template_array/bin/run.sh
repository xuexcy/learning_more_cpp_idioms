#!/bin/bash
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/12/17 16:41:07
# Desc    :
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

./expression_template_array_profile.sh
./expression_template_array_benchmark.sh








