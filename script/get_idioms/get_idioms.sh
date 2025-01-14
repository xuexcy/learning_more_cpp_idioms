#!/bin/bash
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/10 22:50:34
# Desc   :
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}

tmp_idioms=tmp_idioms_from_web.txt
#python get_idioms.py > $tmp_idioms
cat $tmp_idioms | python process_idioms.py > idioms_from_web.txt








