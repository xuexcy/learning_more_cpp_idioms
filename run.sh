#!/bin/bash
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/10/29 22:22:06
# Desc    :
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

binary_dir=build/output/bin
cd $binary_dir

for file in *
do
    ./$file
    if [ $? -ne 0 ]; then
        echo $file "execute failed"
        exit 1
    fi
done






cd -


