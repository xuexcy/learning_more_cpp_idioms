/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 16:10:50
# Desc   :
########################################################################
*/
#pragma once

class Stream {
public:
  Stream();
  ~Stream();
  void say_hi();
};  // class Stream
extern Stream& stream;

struct StreamInitializer {
  StreamInitializer();
  ~StreamInitializer();
};
static StreamInitializer initializer;



