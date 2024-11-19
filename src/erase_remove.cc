/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/19 23:00:51
# Desc    :
########################################################################
使用 std::remove 从 std::vector 删除元素时，只是将需要删除的元素移到 std::vector 后面，而没有真正的删除元素
因为 std::remove 使用的是 ForwardIterator，只有向前检测的功能， 并不知道如何从容器中删除元素

@solution: std::remove 返回一个 iterator 指向第一个可以被删除的元素时，只需要使用 std::vector::erase 从 这个
iterator 开始删除元素即可
*/

#include <iterator>
#include <vector>

#include "utils/main_decorator.h"
template <class ForwardIt, class T = typename std::iterator_traits<ForwardIt>::value_type>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& value) {
    first = std::find(first, last, value);
    if (first != last) {
        for (ForwardIt i = first; i != last; ++i) {
            if (*i != value) {
                *first = std::move(*i);
                ++first;
            }
        }
    }
}
template <class T>
inline void remove(std::vector<T>& v, const T& item) {
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}

void run() {
    std::vector<int> v{1, 99, 2, 99, 3};
    std::println("{}", v);
    remove(v, 99);
    std::println("{}", v);
}

int main() {
    utils::MainDecorator::Access();
    run();
    return 0;
}

