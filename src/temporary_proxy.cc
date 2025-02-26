/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/25 23:07:10
# Desc   :
########################################################################
当通过 operator[] 获得元素的引用后，无法区分对元素的读写，比如在读元素时 ++read_char_cnt_, 在写元素时 ++write_char_cnt_
std::string s{"hello"};
char& c = s[0];
// 读
std::cout << c < <std::endl;
++read_char_cnt_;
// 写
c = 'a';
++write_char_cnt_;;


为此，我们希望将元素的读写绑定到两个不同函数，以执行不同的逻辑。
@solution: 将元素绑定到一个 proxy 类中，由 proxy 类提供读写接口
绑定方式有两种:
  1. 绑定原实例和元素的 index
  2. 绑定元素的引用
*/

#include <iostream>
#include <ostream>
#include <print>
#include <stdexcept>
#include <string>
#include "utils/main_decorator.h"
namespace counting_string {
class CountingString : public std::string {
public:
  class Proxy {
  public:
    Proxy(CountingString* cs, size_t index): cs_(cs), ch_(cs->at(index)) {};
    // read
    operator const char& () const {  // 隐式转换
      ++cs_->read_char_cnt_;
      return ch_;
    }
    // write
    Proxy& operator=(char c) {
      ++cs_->write_char_cnt_;
      ch_ = c;
      return *this;
    }
  private:
    CountingString* cs_{nullptr};
    char& ch_;
  };  // class Proxy

public:
  CountingString(const std::string& s): std::string(s) {}
  Proxy operator[](size_t index) {
    return Proxy{this, index};
  }
private:
  size_t read_char_cnt_{0};
  size_t write_char_cnt_{0};

  friend struct std::formatter<CountingString>;
};  // class CountingString
}  // namespace counting_string

template <>
struct std::formatter<counting_string::CountingString> : std::formatter<std::string> {
  auto format(const counting_string::CountingString& cs, format_context& ctx) const {
    return formatter<string>::format(
      std::format(
        "str={}, read_char_cnt={}, write_char_cnt={}",
        static_cast<std::string>(cs), cs.read_char_cnt_, cs.write_char_cnt_),
      ctx);
  }
};

namespace counting_string {
void run() {
  CountingString cs{"hello"};
  char c = cs[1];
  std::println("c={}, cs=[{}]", c, cs);
  std::cout << cs[1] << std::endl;
  std::println("cs=[{}]", cs);
  std::cout << cs[1] << std::endl;
  std::println("cs=[{}]", cs);

  cs[1] = 'a';
  std::println("cs=[{}]", cs);
  cs[1] = 'b';
  std::println("cs=[{}]", cs);
}
}  // namespace counting_string

namespace undo_string {
class UndoString : public std::string {
public:
  class Proxy {
  public:
    Proxy(UndoString* us, size_t index): us_(us), index_(index), ch_(us->at(index)) {};
    // read
    operator const char& () const {  // 隐式转换
      return ch_;
    }
    Proxy& operator=(char c) {
      us_->old_char_ = ch_;
      us_->been_written_pos_ = index_;

      ch_ = c;
      return *this;
    }
  private:
    UndoString* us_{nullptr};
    size_t index_{0};
    char& ch_;
  };  // class Proxy
public:
  UndoString(const std::string& s): std::string(s) {}
  Proxy operator[] (size_t index) {
    return {this, index};
  }
  void undo() {
    if (-1 == been_written_pos_) {
      throw std::runtime_error("no char has been written");
    }
    std::string::at(been_written_pos_) = old_char_;;

    old_char_ = 0;
    been_written_pos_ = -1;
  }
private:
  int been_written_pos_{-1};
  char old_char_{0};
};  // class UndoString
}  // namespace undo_string

template <>
struct std::formatter<undo_string::UndoString> : std::formatter<std::string> {
  auto format(const undo_string::UndoString& us, format_context& ctx) const {
    return formatter<string>::format(
      std::format("{}", static_cast<std::string>(us)), ctx);
  }
};

namespace undo_string {
void run() {
  UndoString us{"hello"};
  std::println("us={}", us);
  us[1] = 'a';
  std::println("us={}", us);
  us.undo();
  std::println("us={}", us);
}
}  // namespace undo_string

int main() {
  utils::MainDecorator::Access();
  counting_string::run();
  std::println();
  undo_string::run();
  return 0;
}
