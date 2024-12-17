/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/12/17 17:11:52
# Desc    :
########################################################################
*/
#pragma once

struct foo {
};  // struct foo

inline foo* create_foo() {
  return new foo;
}
inline void destroy_foo(foo* f) {
  delete f;
}
