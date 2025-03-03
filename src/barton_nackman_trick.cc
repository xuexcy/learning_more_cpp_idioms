/**
在不依赖namespace和函数模板重载的情况下，重载操作符

在 John Barton 和 Lee Nackman 的年代(1994年)，存在如下两种情况：
1. c++ 不支持函数模板重载
2. 很多编译器实现还不支持 namespace
在实现 operator== 来判断 class List 是否相等时，如果 operator== 定义在全局作用域，则无法为其他 class
(如 class Queue) 实现 operator==(情况1), 如果 operator== 定义在特定的作用域(比如 namespace list),
在当时又有很多编译器还不支持 namespace

@solution: 将 operator== 定义在 class 内部，这样在 operator== 就没有了模板参数(模板参数在 class List), 如
class ListV2, 在实例化模板时,编译器就会将非模板函数注入全局作用域，比如实例化 class ListV2<int>时，就会注入
bool operator==(const ListV2<int>& lhs, const ListV2<int>& rhs) 到全局作用域
*/

#include <cassert>

#include "utils/main_decorator.h"

template <typename T>
class List {};

template <typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs) {
    return true;
}

template <typename T>
class Queue {};

// 在当时的c++标准中, 不支持下面的operator==<T>重载
template <typename T>
bool operator==(const Queue<T>& lhs, const Queue<T>& rhs) {
    return true;
}

// solution
template <class T>
class ListV2 {
public:
    friend bool operator==(const ListV2<T>& lhs, const ListV2<T>& rhs) {
        return true;
    }
};

template <class T>
class Value {
public:
    Value(T val): val_(val) {}
    bool operator==(const Value<T>& rhs) {
        return val_ == rhs.val_;
    }
private:
    T val_;
};

// 结合 crtp(curiously_recurring_template_pattern) 实现 operator== 的重载接口
template <typename T>
class EqualityComparable {
public:
    friend bool operator==(const T& lhs, const T& rhs) {
        return lhs.equalTo(rhs);
    }
    friend bool operator!=(const T& lhs, const T& rhs) {
        return !lhs.equalTo(rhs);
    }
};

// idiom: curiously_recurring_template_pattern
class ValueType : private EqualityComparable<ValueType> {
public:
    explicit ValueType(int val): val_(val) {}
    bool equalTo(const ValueType& other) const {
        return this->val_ == other.val_;
    }
private:
    int val_{0};
};

int main() {
    utils::MainDecorator::Access();
    {
        List<int> l1;
        List<int> l2;
        assert(l1 == l2);
    }
    {
        ListV2<float> l1;
        ListV2<float> l2;
        assert(l1 == l2);
    }
    {
        Value v(10);
        assert(v == 10);
        assert(10 == v);
        assert(v != 11);
        assert(11 != v);
    }
    {
        ValueType v1(10);
        ValueType v2(10);
        ValueType v3(11);
        assert(v1 == v2);
        assert(v1 != v3);
    }
    return 0;
}
