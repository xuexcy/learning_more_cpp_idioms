/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/07 20:26:04
# Desc   :
########################################################################
polymorphic value-type: 多态 值类型
通过非继承的方式，提供多态

@refs:
https://zhuanlan.zhihu.com/p/529942637
https://github.com/jbcoe/value_types
https://www.youtube.com/watch?v=sjLRX4WMvlU
https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0201r5.html
https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0201r5.html
*/

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

class copyable_hashable {
private:
  template <class T>
  static constexpr bool is_in_place_type = false;
  template <class T>
  static constexpr bool is_in_place_type<std::in_place_type_t<T>> = true;

  struct impl;

  template <class T>
  struct impl_model;

  template <class T>
  using stored = impl_model<std::remove_cvref_t<T>>;

  std::unique_ptr<impl> data_{nullptr};

public:
  // 可拷贝 + 可比较相等 + 可 hash
  template <class T>
  static constexpr bool storable = (std::copy_constructible<T>
    && std::equality_comparable<T>
    && requires (const T& val) {
      std::invoke_r<std::size_t>(std::hash<T>(), val);
    }
  );

  // 定义: 默认构造函数
  constexpr copyable_hashable() noexcept = default;
  // 声明: 拷贝构造函数
  constexpr copyable_hashable(const copyable_hashable&);
  // 定义: 默认移动构造函数
  constexpr copyable_hashable(copyable_hashable&&) noexcept = default;

  // 定义: 带参构造函数
  // 模板参数: 类型 T，构造类型 T 的参数列表 class... Args
  // requires:
  //   - T 必须 storable
  //   - T 必须可以由 class.. Args 构造
  // 构造参数: std::in_place_type_t<T>, 构造类型 T 的参数 Args&&... args
  template <class T, class... Args>
  requires (
    storable<std::remove_cvref_t<T>>
    && std::constructible_from<std::remove_cvref_t<T>, Args...>
  )
  constexpr copyable_hashable(std::in_place_type_t<T>, Args&&... args) :
    data_(std::make_unique<stored<T>>(std::forward<Args>(args)...)) {

  }

  // 定义: 带参构造函数
  // 模板参数: 类型 T
  // requires:
  //   - T 不是 copyable_hashable 类型
  //   - T 不是 std::in_place_type_t 封装的类型
  //   - T 必须 storable
  //   - T 必须可以由 T 构造(即具有拷贝构造函数)
  // 构造参数: T 的实例 T&& value
  template <class T>
  requires (
    !std::is_same_v<std::remove_cvref_t<T>, copyable_hashable>
    && !is_in_place_type<std::remove_cvref_t<T>>
    && storable<std::remove_cvref_t<T>>
    && std::constructible_from<std::remove_cvref_t<T>, T>
  )
  constexpr copyable_hashable(T&& value):
    copyable_hashable(std::in_place_type<T>, std::forward<T>(value)) {

  }

  // 定义: 拷贝赋值操作符
  constexpr copyable_hashable& operator=(copyable_hashable rhs) noexcept {
    swap(rhs);
    return *this;
  }

  // 定义: 左值才能调用的 get 函数，返回 T 的引用
  // requires:
  //   - T 不能是引用类型
  template <class T>
  requires (!std::is_reference_v<T>)
  constexpr T& get() & noexcept {
    return dynamic_cast<stored<T>&>(*data_).value;
  }

  // 定义: const 左值才能调用的 get 函数，返回 T 的 const 引用
  // requires:
  //   - T 不能是引用类型
  template <class T>
  requires (!std::is_reference_v<T>)
  constexpr const T& get() const& noexcept {
    return dynamic_cast<const stored<T>&>(*data_).value;
  }


  // 定义: 右值才能调用的 get 函数，返回 T 的右值引用
  // requires:
  //   - T 不能是引用类型
  template <class T>
  requires (!std::is_reference_v<T>)
  constexpr T&& get() && noexcept {
    return std::move(dynamic_cast<stored<T>&>(*data_).value);
  }

  // 定义: const 右值才能调用的 get 函数，返回 T 的 const 右值引用
  // requires:
  //   - T 不能是引用类型
  template <class T>
  requires (!std::is_reference_v<T>)
  constexpr T&& get() const&& noexcept {
    return std::move(dynamic_cast<const stored<T>&>(*data_).value);
  }

  // 定义: 获取存储的数据的地址 T*
  template <class T>
  constexpr T* get_ptr() noexcept {
    // const auto ptr = dynamic_cast<stored<T>*>(data_.get());
    // return ptr ? std::addressof(ptr->value) : nullptr;

    // 非 const 函数调用 const 函数
    return const_cast<T*>(static_cast<const copyable_hashable&>(*this).get_ptr<T>());
  }


  // 定义: 获取存储的数据的地址 const T*
  template <class T>
  constexpr const T* get_ptr() const noexcept {
    const auto ptr = dynamic_cast<const stored<T>*>(data_.get());
    return ptr ? std::addressof(ptr->value) : nullptr;
  }

  constexpr const std::type_info& type() const noexcept;

  constexpr void swap(copyable_hashable& other) noexcept {
    data_.swap(other.data_);
  }

  constexpr bool operator==(const copyable_hashable&) const;
  friend struct std::hash<copyable_hashable>;

};  // class copyable_hashable


struct copyable_hashable::impl {
  constexpr virtual ~impl() noexcept = default;
  constexpr virtual std::unique_ptr<impl> clone() const = 0;
  constexpr virtual const std::type_info& type() const noexcept = 0;
  constexpr virtual bool equals(const impl&) const = 0;
  constexpr virtual std::size_t hash() const = 0;
};  // struct copyable_hashable::impl

// 定义： copyable_hashable 拷贝构造函数
constexpr copyable_hashable::copyable_hashable(const copyable_hashable& other):
  data_(other.data_ ? other.data_->clone() : nullptr) {
}

constexpr const std::type_info& copyable_hashable::type() const noexcept {
  return data_ ? data_->type() : typeid(void);
}

constexpr bool copyable_hashable::operator==(const copyable_hashable& other) const {
  const bool neither_null = data_ && other.data_;
  return neither_null ? data_->equals(*other.data_) : data_ == other.data_;
}

template <class T>
struct copyable_hashable::impl_model : public impl {
  T value;

  template <class... Args>
  constexpr impl_model(Args&&... args) : value(std::forward<Args>(args)...) {}

  constexpr std::unique_ptr<impl> clone() const override {
    return std::make_unique<impl_model>(value);
  }

  constexpr const std::type_info& type() const noexcept override {
    return typeid(T);
  }

  constexpr bool equals(const impl& other) const override {
    if (const auto other_ptr = dynamic_cast<const impl_model*>(&other)) {
      return value == other_ptr->value;
    }
    return false;
  }

  constexpr std::size_t hash() const override {
    return std::invoke_r<std::size_t>(std::hash<T>(), value);
  }
};  // struct copyable_hashable::impl_model

constexpr void swap(copyable_hashable& lhs, copyable_hashable& rhs) noexcept {
  lhs.swap(rhs);
}

template <>
struct std::hash<copyable_hashable> {
  constexpr std::size_t operator()(const copyable_hashable& val) const {
    return val.data_ ? val.data_->hash() : 0uz;
  }
};  // struct std::hash<copyable_hashable>
