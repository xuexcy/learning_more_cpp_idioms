/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 17:04:48
# Desc   :
########################################################################
*/

#include "stream.h"

#include <print>

#include "cpp_utils/util.h"

static int nifty_counter{0};
static char stream_buf[sizeof(Stream)];
Stream& stream = reinterpret_cast<Stream&>(stream_buf);

Stream::Stream() {
  PRINT_CURRENT_FUNCTION_NAME;
}
Stream::~Stream() {
  PRINT_CURRENT_FUNCTION_NAME;
}
void Stream::say_hi() {
  std::println("hi, I'm stream, an instance of class 'Stream'");
}

StreamInitializer::StreamInitializer() {
  if (0 == nifty_counter) new (&stream_buf)(Stream);
  ++nifty_counter;
  std::println("nifty_counter: {}", nifty_counter);
}
StreamInitializer::~StreamInitializer() {
  --nifty_counter;
  if (0 == nifty_counter) {
    stream.~Stream();
  }
  std::println("nifty_counter: {}", nifty_counter);
}



