/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/02 16:56:09
# Desc   :
########################################################################
在实现模板函数或类时，可能需要多个模板参数，我们可以提供默认的模板值以减少代码量
对于那些已有的模板，我们可以通过封装的方式来提供模板值
*/

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>

/*
https://en.cppreference.com/w/cpp/container/map
template<
    class Key,
    class T,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, T>>
> class map;

std::map 有 4 个模板参数，其中 Compare 和 Allocator 有默认值，如果我们想要定义一个和业务相关的
map，其 key 一定 是 std::string，我们就可以封装自己的 map
*/
template <class T>
class Dictionary {  // 其实这里我们也可以继承 std::map<std::string, T>
public:
  using type = std::map<std::string, T>;
  // 注意 const std::string
  using element_type = type::value_type;  // std::pair<const std::string, T>
  using key_type = type::key_type;
  using value_type = type::mapped_type;

  // https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.1/stl__function_8h-source.html
  // Select1st
  struct KeySelector{
    // 注意，这里入参是 const 但 return 不是，因为 element_type.first 是 const std::string（map 的 key不能被修改）
    const key_type& operator()(element_type& elem) const { return elem.first; }
    const key_type& operator()(const element_type& elem) const { return elem.first; }
  };  // KeySelector
  struct ValueSelector {
    value_type& operator()(element_type& elem) const { return elem.second; }
    const value_type& operator()(const element_type& elem) const { return elem.second; }
  };  // ValueSelector
};  // class Dictionary
template <class T>
using Dictionary_t = typename Dictionary<T>::type;
template <class T>
using DictionaryKeySelector = typename Dictionary<T>::KeySelector;
template <class T>
using DictionaryValueSelector = typename Dictionary<T>::ValueSelector;

void run() {
  Dictionary_t<std::string> name_to_address;
  Dictionary_t<int> name_to_age;
  name_to_address["wang"] = "China";
  name_to_address["wang"] = "18";

  name_to_address["john"] = "USA";
  name_to_address["john"] = "19";
  // print names
  std::transform(name_to_address.begin(), name_to_address.end(),
      std::ostream_iterator<std::string>(std::cout, "\n"), DictionaryKeySelector<std::string>());
  std::transform(name_to_address.begin(), name_to_address.end(),
      std::ostream_iterator<std::string>(std::cout, "\n"), DictionaryValueSelector<std::string>());
  std::transform(name_to_age.begin(), name_to_age.end(),
      std::ostream_iterator<int>(std::cout, "\n"), DictionaryValueSelector<int>());
}

int main() {
  run();
  return 0;
}
