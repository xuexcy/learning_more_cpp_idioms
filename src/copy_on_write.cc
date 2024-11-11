/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/11 11:59:38
# Desc    :
########################################################################
写时复制 copy on write: for lazy copy. 用户在拷贝数据时，仅仅是增加一下数据的计数器，这样，拷贝前后的两个变量还是共享的数据指针，如果用户在后续使用中没有改变
数据，那么拷贝前后的数据都是共享的、没有发生改变的，这样就节省了一次真实数据的拷贝，如果用户更改了数据，那么此时才真正的
将数据拷贝一份并更改。简单来说就是你不改数据，我们就共享读，你要改的时候再拷贝一份自己去改吧。

比如我买了一本书(ref_count初始化为1)，你说你想看，那我俩就一起看(ref_count += 1)，你看了一段时间，书一直都是我俩
共享的状态，后来，你说你想在书上面做个笔记，我说那你按我这个书的 ISN 码去重新买一本(copy and set new ref_count = 1)
再做笔记吧，我这本我拿走了(ref_count -= 1)

@bad_case: wikibooks 上写明确的告诉我们，按照文中的写法是有问题的，也就是下面的 solution_with_bug，同时也说只要让类的开发者
明确的指出那些操作需要 copy 即可解决问题
在 https://www.codeproject.com/Tips/5261583/Cplusplus-Copy-On-Write-Base-Class-Template 中的实现方式是:

1. 定义 COWBase<T>，在里面实现 clone_if_needed，这个函数就是让用户决定那些操作是需要 copy 的，比如 setXXXX 函数
2. 定义一个用户使用的类 T
3. 定义在代码中实际使用的类 COW_T : private COWBase<T>，同时定义各种函数，并在需要 copy 的函数中调用 clone_if_need
使用这种方式的前提是 T 和 COW_T 都是用户自己的代码，如果要对一个第三方的 T 封装，上述方法可能就不太合适，因为 COW_T 中
缺少了一些直接使用 T 中的函数的接口，比如构造函数，也就是不能写成 COW_T t(xxxx); 除非将 T 的构造函数抄一遍

@solution: 我在 solution 中给出了一种解决上诉 COWBase<T> 存在的问题的一种解决办法
1. 在 COWBase<T> 中通过变长模板构造函数，这样 COWBase<T_Impl> 就可以直接使用 T_Impl 构造函数
    template <class... Args>
        COWBase(Args&&... args): m_ptr(std::make_shared<T>(std::forward<Args>(args)...)) {
    }
2. 在 T 中使用 using COWBase<T_Impl>::COWBase<T_Impl> ，这样继承类也就可以使用基类的构造函数了
3. 在 COWBase<T> 中，将 const T* ptr() const 设置成 public，这样对于 COW_T 的实例使用 T 的普通函数时，
就可以分成两种方式:
    a. 对于需要 copy 的函数(个人理解就是非 const 函数)，这些函数需要在 T 中重新实现，然后直接以 t.set_xxx(a) 的方式调用
        void set_xxx(int a) {
            clone_if_needed();
            a_ = a;
        }
    b. 对于不需要 copy 的函数，比如 get_xxx 函数，直接使用 t.ptr()->get_xxx() 的方式调用
*/

#include <memory>
#include <print>
#include <string>
#include <utility>

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"

// COWPtr: copy on write pointer

namespace solution_with_bug {
template <class T>
class COWPtr {
private:
    std::shared_ptr<T> m_ptr;
    void clone_if_needed() {  // 将当前 m_ptr 分离/独立出来
        if (m_ptr.use_count() <= 1) {
            // 如果 count == 0, 没数据，不用 clone
            // 如果 count == 1, 没有其他地方使用，不用 clone
            return;
        }
        PRINT_CURRENT_FUNCTION_NAME;
        std::shared_ptr<T> old = m_ptr;
        m_ptr = std::make_shared<T>(*old);
    }
public:
    COWPtr(T* t): m_ptr(t) {
        PRINT_CURRENT_FUNCTION_NAME;
    }
    COWPtr(const std::shared_ptr<T>& ref_ptr): m_ptr(ref_ptr) {}
    COWPtr(const COWPtr& rhs): m_ptr(rhs.m_ptr) {}
    const T& operator*() const {
        PRINT_CURRENT_FUNCTION_NAME;
        return *m_ptr;
    }
    T& operator*() {
        PRINT_CURRENT_FUNCTION_NAME;
        clone_if_needed();
        return *m_ptr;
    }
    const T* operator->() const {
        PRINT_CURRENT_FUNCTION_NAME;
        return m_ptr.operator->();
    }
    T* operator->() {
        PRINT_CURRENT_FUNCTION_NAME;
        clone_if_needed() ;
        return m_ptr.operator->();
    }
    auto use_count() const { return m_ptr.use_count(); }
};
/**
可以看到，我们将 ch = s1[1] = '5' ，按照 copy on write 的预期，s1 应该等于 "153" , s2 等于 "123"
但实际上并没有发生 copy, 因为一开始我们就获取了 char& ch，后续改变 ch 的时候，s1 和 s2 并不知情，所以出现了问题
解决这个问题的办法在 namespace solution 中，我们需要让用户自己决定那些接口需要主动调用 copy (也就是 非 const 就 copy，
const 就不 copy)

@stdout:
solution_with_bug::COWPtr<std::string>::COWPtr(T *) [T = std::string]
T &solution_with_bug::COWPtr<std::string>::operator*() [T = std::string]
const T &solution_with_bug::COWPtr<std::string>::operator*() const [T = std::string]
123 use_count: 2
const T &solution_with_bug::COWPtr<std::string>::operator*() const [T = std::string]
123 use_count: 2
const T &solution_with_bug::COWPtr<std::string>::operator*() const [T = std::string]
153 use_count: 2
const T &solution_with_bug::COWPtr<std::string>::operator*() const [T = std::string]
153 use_count: 2
*/
void run() {
    {
        COWPtr<std::string> s1(new std::string("123"));
        // no copy here because of 1 == s1.m_ptr.use_count
        auto& ch = (*s1)[1];  // call T& operator*(), get type char&
        COWPtr<std::string> s2(s1);
        // *s1 会调用 T& operator*(), 所以下面这行会直接 copy，不是我们想要的
        // std::println("{} use_count: {}", *s1, s1.use_count());
        // std::println("{} use_count: {}", *s2, s2.use_count());
        const auto& s1_ref = s1;
        const auto& s2_ref = s2;
        std::println("{} use_count: {}", *s1_ref, s1_ref.use_count());
        std::println("{} use_count: {}", *s2_ref, s2_ref.use_count());
        ch = '5';
        std::println("{} use_count: {}", *s1_ref, s1_ref.use_count());
        std::println("{} use_count: {}", *s2_ref, s2_ref.use_count());

    }
}
}  // namespace solution_with_bug


namespace solution {
template <class T>
class COWBase {
    friend void run();
public:
    template <class... Args>
    COWBase(Args&&... args): m_ptr(std::make_shared<T>(std::forward<Args>(args)...)) {
    }
    const T* ptr() const {
        return m_ptr.get();
    }
protected:
    void clone_if_needed() {
        if (m_ptr.use_count() <= 1) {
            return;
        }
        PRINT_CURRENT_FUNCTION_NAME;
        std::shared_ptr<T> old = m_ptr;
        m_ptr = std::make_shared<T>(*old);
    }
    T* ptr() {
        return m_ptr.get();
    }
    auto use_count() const {
        return m_ptr.use_count();
    }
private:
    std::shared_ptr<T> m_ptr;
};

struct Book {
    std::string name;
    std::string content;
    std::string note;  // 记笔记
    Book(
        const std::string& name_, const std::string& content_, const std::string& note_):
        name(name_), content(content_), note(note_) {}
    std::string string() const {
        return std::format("[name: {}, content: {}, note: {}]", name, content, note);
    }
    void set_name(const std::string& name) {
        PRINT_CURRENT_FUNCTION_NAME;
        this->name = name;
        std::println("{}", name);
    }
    const std::string& get_name() const { return this->name; }
    void set_content(std::string content) {
        PRINT_CURRENT_FUNCTION_NAME;
        this->content = content;
        std::println("{}", content);
    }
    const std::string& get_content() const { return this->content; }
    void set_note(std::string note) {
        PRINT_CURRENT_FUNCTION_NAME;
        this->note = note;
        std::println("{}", note);
    }
    const std::string& get_note() const { return this->note; }
};

class COWBook: public COWBase<Book> {
public:
    // using Book::Book;  // 使用 Book 的构造函数
    using COW = COWBase<Book>;
    using COW::COW;
    void set_name(const std::string& name) {
        COW::clone_if_needed();
        COW::ptr()->set_name(name);
    }
    void set_content(const std::string& content) {
        COW::clone_if_needed();
        COW::ptr()->set_content(content);
    }
    void set_note(const std::string& note) {
        COW::clone_if_needed();
        COW::ptr()->set_note(note);
    }
};

void run() {
    Book book("Bible", "Oh my Jesus", "New book with empty note");
    std::string content_for_update{"Oh my Jesus, 我佛慈悲"};
    std::string note_for_update{"东西结合，邪魔退让"};
    auto print = [](const auto& name, const auto& tb) {
        std::println("variable: {}, use_count: {}, book value: {}",
            name, tb.use_count(), tb.ptr()->string());
    };
    #define PRINT(var) print(#var, var);
    // 下面的输出中，在 COWBook tb2(tb); 时，tb 和 tb2 共用一份数据，两者的 use_count = 2
    // 一旦其中一个 set_xxx，就会发生 copy （在输出中可以看到 clone_if_needed 被调用），之后两者的 use_count = 1
    {
        std::println("\nwrite tb twice");
        COWBook tb(book);
        PRINT(tb);

        COWBook tb2(tb);
        PRINT(tb);
        PRINT(tb2);

        tb.set_content(std::format("content updated from tb: {}", content_for_update));
        PRINT(tb);
        PRINT(tb2);

        tb.set_note(std::format("note updated from tb: {}", note_for_update));
        PRINT(tb);
        PRINT(tb2);
    }
    {
        std::println("\nwrite tb2 twice");
        COWBook tb(book);
        PRINT(tb);

        COWBook tb2(tb);
        PRINT(tb);
        PRINT(tb2);

        tb2.set_content(std::format("content updated from tb2: {}", content_for_update));
        PRINT(tb);
        PRINT(tb2);

        tb2.set_note(std::format("note updated from tb2: {}", note_for_update));
        PRINT(tb);
        PRINT(tb2);
    }
    {
        std::println("\nwrite tb once, write tb2 once");
        COWBook tb(book);
        PRINT(tb);

        COWBook tb2(tb);
        PRINT(tb);
        PRINT(tb2);

        tb2.set_content(std::format("content updated from tb2: {}", content_for_update));
        PRINT(tb);
        PRINT(tb);
        PRINT(tb2);

        tb.set_note(std::format("note updated from tb: {}", note_for_update));
        PRINT(tb);
        PRINT(tb2);
    }
    #undef PRINT
}
}  // namespace solution

int main() {
    utils::MainDecorator::Access();
    solution_with_bug::run();
    solution::run();
    return 0;
}
