/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 17:29:40
# Desc   :
########################################################################
*/

#include "b.h"

#include "cpp_utils/util.h"
#include "nifty_counter_deps/stream.h"

void B::use_global_stream() const {
  PRINT_CURRENT_FUNCTION_NAME;
  ::stream.say_hi();
}
