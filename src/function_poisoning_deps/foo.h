/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/12/17 17:13:58
# Desc    :
########################################################################
*/
#pragma once

#include <memory>
#include "__details/foo.h"

struct foo_deleter {
  void operator()(foo* f) {
    destroy_foo(f);
  }
};  // struct foo_deleter
using unique_foo = std::unique_ptr<foo, foo_deleter>;

inline unique_foo create_unique_foo() {
  return unique_foo(create_foo());
}

#pragma GCC poison create_foo



