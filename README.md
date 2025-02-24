# [More C++ Idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)
## Build And Run
- `env_variable.sh`: 设置环境变量，如 cmake 工作目录、可执行文件存放的 bin 文件夹目录等
- `build.sh`: 编译所有 src/*.cc
- `run.sh`: 运行所有编译后的可执行文件，并将结果输出到 src 目录
- `build_one.sh`: 编译 src 目录下的一个 .cc 文件，具体编译哪一个文件取决于 `env_variable.sh` 中的 `default_execute_file` 环境变量, 这个脚本主要是自己修改、测试某个 .cc 代码的时候使用，该文件里没有调用 cmake，如果编译的相关依赖发生改变，请重新执行 `build.sh` 生成新的 makefile 文件
- `run_one.sh`: 执行一个编译结果，同 `build_one.sh` 一样，具体哪个取决于 `env_variable.sh` 中的 `default_execute_file`
- 其他:
  - `run.sh` 执行后的输出结果在 stdout 目录下（可以通过 `env_variable.sh` 进行配置）
  - `src/*.cc` 是各个 idiom main 文件

## Idioms
- [address_of](address_of.cpp): ⭐ 在 `&` 操作符被重载的情况下，获取 object 的地址
    > `&` 操作符作为一元操作符，用于获取 object 的地址，作为二元操作符，表示两个 object 逻辑与。如果  `&` 一元操作符被重载后不表示 object 的地址，那就改变了操作符含义。我们可能会重载 `operator+`，但是并不会改变 `+` 的含义，比如小时 + 分 + 秒，依旧表示时间上的相加。但是什么情况下会改变 `operator&` 的含义呢？如果改变了其含义，那和 `#define false true` 有什么区别，这不是搞事情吗？
- [algebraic_hierarchy](algebraic_hierarchy.cpp): ⭐ 仅暴露抽象基类并在用户代码中进行使用， 如使用抽象基类`Number`表达数字， 定义继承类虚数`Complex`和实数`RealNumber`，但是，在用户代码中只会使用基类而不是使用继承类
    ```cpp
    // 如下代码中只使用了 Number 基类
    Number n1 = Number::makeComplex(1， 2);
    Number n2 = Number::makeRealNumber(5);
    Number n3 = n1 + n2;
    n3 = n2;
    ```
- [attach_by_initialization](src/attach_by_initialization.cc): ⭐⭐⭐⭐ 在用户代码 main 函数开始前，通过全局变量 global variable 执行一些工具类的初始化流程，比如 [g_main_decorator](src/utils/main_decorator.h)，在 main 开始前和结束后打印一些信息
- [attorney_client](src/attorney_client.cc): ⭐⭐ 如果 `class A(friend class B)`，则 class B 可以访问 class A 的所有私有成员。如果我们希望 class B 只能访问 class A 的一部分私有成员，可以通过代理类 `class Attorney` 控制友元类对私有成员的访问范围，`class A(friend class Attorney); class Attorney(friend class B)`，这样只要在 class Attorney 中设置允许 class B 访问 class A 的私有成员的接口即可
- [barton_nackman_trick](src/barton_nackman_trick.cc): ⭐⭐⭐ 在模板类中将 operator 设置成 friend，解决 operator 重载带模板参数的问题
- [base_from_member](src/base_from_member.cc): ⭐⭐ 基类先于继承类成员构造，若基类构造依赖继承类的成员，构造结果将可能不符合预期。通过将基类依赖的成员提取到单独的 class Container， 再由继承类控制继承顺序， 来解决此类问题
- [boost_mutant](src/boost_mutant.cc): ⭐⭐ 对内存布局一致的两个 class，可以从同一个对象地址开始读取数据
- [calling_virtuals_during_initialization](src/calling_virtuals_during_initialization.cc): ⭐⭐⭐⭐⭐ 通过two-phase-initialization，实现类似在 Base 类的构造函数中调用继承类的虚函数的逻辑
- [capability_query](src/capability_query.cc): ⭐⭐⭐ 在运行时通过 dynamic_cast 将 object 指针转换为接口类 class Interface 指针， 根据转换是否成功来判断这个对象是否实现了该接口(即是否继承了接口类)
- [checked_delete](src/checked_delete.cc): ⭐⭐⭐ 在不知道 class 完整定义时调用 `delete obj_ptr`， 代码编译会出现 warning 并编译成功，但运行结果是未定义，通过在 delete 前使用 `char[sizeof(T)?1:-1]` 让在此种情况下编译失败，以防止运行时结果不符合预期
- [clear_and_minimize](src/clear_and_minimize.cc): ⭐⭐⭐⭐⭐ 容器在占用大量内存(capacity很大)且 clear 后，程序不会回收这些内存，造成内存浪费，通过空的容器与其 swap 可以回收，比如 `std::vector<int>().swap(v)`
- [coercion_by_member](src/coercion_by_member.cc): ⭐⭐ Derived* 可以转换成 Base*，但是封装后的不行，比如 `std::unique_ptr<Base> b = std::unique_ptr<Derived>()`，因为两个 unique_ptr 没有继承关系。通过模板封装 Derived 并实现拷贝构造和赋值构造，可以主动将 Derived* 转换为 Base*，如模板类 `Ptr<class T>` 实现相关构造函数后，可以将 Derived 指针转换为基类指针 `Ptr<Base> b = Ptr<Derived>()`
- [computational_constructor](src/computational_constructor.cc): ⭐ 对于不支持 NRVO(named return value optimization) 的编译器，将函数中的计算并 return obj 的逻辑，改成 return Object(xxx) 的方式(即由构造函数实现计算逻辑并直接 return 构造结果)，也就是通过构造 + RVO 的方式来替代 NRVO
- [concrete_data_type](src/concrete_data_type.cc): ⭐⭐⭐ 控制类实例的作用域和生命周期;通过将构造函数或析构函数设置成 protected 来禁止在桟上构造类实例；通过将 new 设置成 private 或 protected，来禁止在堆上构造类实例
- [construct_on_first_use](src/construct_on_first_use.cc): ⭐⭐⭐⭐ class static variable 可能在使用时还未被构造，这样可能导致使用结果不符合预期，通过将其放到 class static/member function 中，在 function 中确保 variable 使用前完成了构造，即 `static Bar bar_;` -> `Bar& get_bar() { static Bar bar; return bar; }`
    > [more effective c++ 条款 04](https://github.com/xuexcy/learning_effective_cpp/blob/main/src/04/main.cc)
- [construction_tracker](src/construction_tracker.cc): ⭐⭐⭐ 在构造函数的列表初始化成员时，不同成员可能会抛出相同类型的异常，通过对每个成员初始化前设置特定 tracker 值，来定位到底时那个成员初始化失败。比如 `b_((++tracker), "x"), "y")`
- [copy_and_swap](src/copy_and_swap.cc): ⭐⭐⭐ 在拷贝赋值函数内使用 swap 来保证 strong guarantee（强烈保证）异常安全
- [copy_on_write](src/copy_on_write.cc): ⭐⭐⭐⭐⭐ 在实例发生 copy 时，通过智能指针共享数据用于数据读取，而不是直接 copy 整个实例中包含的数据，在实例需要 update/write 数据时，再对对应的实例 copy 一份刚刚共享的数据以便写入，到达了 lazy copy 的目的。另外，copy 后也不一定发生 write（比如 write 的前提是需要满足某些业务条件），那么这样就 no copy。 如下代码，假设 `condition1 == true`，那么 copy 发生在 `a.set_x(1)` 时，如果 `codition1 == false; condition2 == true`，那么 copy 发生在 `a.set_y(2)` 时, 这样就是 lazy copy，如果 `condition1 == false; condition2 == false`，那么就不会发生 copy，也就是 no copy。
    ```cpp
    A a(another_a);
    if (condition1) { a.set_x(1); }
    if (condition2) { a.set_y(2); }`
    ```
- [counted_body(Intrusive reference counting)](src/counted_body.cc): ⭐ 对数据指针计数，归 0 时 delete 数据，即 std::shared_ptr<T> 的能力
- [covariant_return_types](src/covariant_return_types.cc): ⭐⭐⭐⭐⭐ 子类 override 基类虚函数，函数返回类型可以是基类虚函数返回类型的子类
    ```cpp
    class A {};
    class B : public A {};
    class Base {public: virtual A* ff(); };
    class Derive : public Base {public: B* ff() override };
    ```
- [curiously_recurring_template_pattern](src/curiously_recurring_template_pattern.cc): ⭐⭐⭐⭐ 奇异递归模板模式， 基类模板参数为继承类，实现编译期多态，即在编译期确定在基类函数中调用哪个子类函数
- [empty_base_optimization](src/empty_base_optimization.cc): ⭐⭐ 空类的 size 为 1, 和包含一个空类实例相比，可以通过继承空类可以优化掉这个浪费调用内存
- [enable_if](src/enable_if.cc): ⭐⭐⭐⭐⭐ 在模板编程时，通过 enable_if 来限制模板参数需要符合一些性质，在 C++ 20 后可以使用 `requires`
- [erase_remove](src/erase_remove.cc): ⭐⭐⭐⭐⭐ 在使用 std::remove 后再使用容器的 erase 来删除元素，因为 std::remove 只是将需要删除的元素挪到容器尾部
- [execute_around_pointer](src/execute_around_pointer.cc): ⭐⭐⭐⭐ 通过将实例封装到一些 class Aspect(有点像 python decorator) 类中，通过重载 operator-> 来执行实例的函数，在函数执行前后通过 Aspect 的构造和析构函数，来执行一些特定的操作，比如在 std::vector.push_back() 执行前后，来 print std::vector.size()
- [expression_template](src/expression_template.cc): ⭐⭐⭐⭐ 通过模板展开将待操作的步骤记录成表达式，以做到延时计算和一些优化。比如计算 `1.2 * a + a * b`(a、b 为 std::array)，则表达式可以记录为 `Add<Multiply<double, std::array>, Multiply<std::array, std::array>>`，这一步骤可以在编译时通过模板展开生成，在需要计算时再调用表达时的计算操作逻辑
    > [expression_template_array](src/expression_template_deps/README.md)
- [final_class](src/final_class.cc): ⭐⭐⭐  限定 class 不允许被继承
    > 在 c++ 11 前，使用 virtual 继承 + friend class
    ```cpp
    class MakeFinal {
        MakeFinal() = default;
        friend class A;
    };
    class A : virtual MakeFinal {};
    ```
    > 在 c++ 11 中，使用 final 关键字
    ```cpp
    class A final {};
    ```
- [free_function_allocator](): **<span style="color:red">TODO</span>**
- [function_poisoning](src/function_poisoning.cc): ⭐⭐⭐ 将一些用户使用不便的函数封装成更易使用的接口，并通过 `#pragma GCC poison $function_name` 来禁止用户使用这些函数，比如 c 或 c++ 11 以前的一些函数，封装成 RAII-friendly 的接口
- [generic_container](src/generic_container.cc): ⭐⭐⭐⭐⭐ 在需要实现一些泛型容器时，将 new/delete array 和 delete array 换成 operator new/delete，分离内存申请与释放和实例构造与析构，以此来减少泛型类的函数依赖限制
- [hierarchy_generation](src/hierarchy_generation.cc): ⭐⭐ 在将多个能力整合到一起时，可以通过多继承来解决(`class MyType: public A, public B, public C`)，也可以通过变长模板展开的方式来实现层次结构的单一继承(`class MyType: public A<B<C<>>>`)
    > 在 rust 中，可以通过 trait 来将多个能力赋予 struct。[rust_traits](src/hierarchy_generation_deps/rust_traits/src/main.rs)
- [implicit_conversion](): **TODO**
- include_guard_macro: ⭐⭐⭐ 在同一个编译单元多次包含同一个头文件违反了 One Definition Rule (ODR)，通过宏来确保头文件只被 include 一次
    ```cpp
    // 通过宏来确保头文件只被 include 一次
    #ifndef MYHEADER_H_
    #define MYHEADER_H_
    xxxx
    #endif  // MYHEADER_H_
    ```
    ```cpp
    // 也可以通过 #pragma once 来确保头文件只被 include 一次，而且这样编译器只会将这个文件打开一次。通过宏的方式编译器可能会多次打开头文件，在第二次及之后判断 #ifndef MYHEADER_H_ 是 false 后才不处理后续的代码
    #pragma once
    xxxx
    ```
- [inline_guard_macro](src/inline_guard_macro.cc): ⭐⭐⭐⭐ 通过将函数定义放置于 .ipp 文件中，在 .h 和 .cpp 文件中通过 `#ifdef ENABLE_INLINE` 和 `#ifndef ENABLE_INLINE` 的方式来决定函数定义是否是 inline，如果是 inline 则定义于 .h 中，如果不是，则定义于 .cpp 中
    ```cpp
    // foo.ipp
    INLINE void foo() { std::println("hi"); }  // define function
    ```
    ```
    // foo.h
    void foo();  // declare function
    #ifdef ENABLE_INLINE
    #define INLINE inline
    #include "foo.ipp"
    #endif
    ```
    ```
    #include "foo.h"
    // foo.cpp
    #ifndef ENABLE_INLINE
    #define INLINE inline
    #include "foo.ipp"
    #endif
    ```
    ```cmake
    // CMakeLists.txt
    add_definitions(-DENABLE_INLINE)
    ```
- [int_to_type](src/int_to_type.cc): ⭐⭐ 通过 `template <int I> struct Int2Type {};` 将编译期的 int 变成一个类型，业务代码可以根据不同的类型在编译期确定执行逻辑，也可以用 `enable_if` 和 `constexpr` 来实现
- [inner_class](src/inner_class.cc): ⭐⭐ 通过在 Derived 中定义不同的 Inner 类来继承不同的 Base，以解决在不同 Base 间含有同名虚函数时，为不同 Base 各自实现虚函数的问题
    > 在 rust 中没有继承，接口能力通过 traits 赋予各个 struct，这样可以在不同的 impl Trait for Struct 中实现同名函数，样例 [rust_traits_same_function_name](src/inner_class_deps/rust_traits_same_function_name/src/main.rs)
- [interface_class](src/interface_class.cc): ⭐⭐⭐⭐⭐ 在类中声明纯虚函数接口，实现接口类 class Interface
- [iterator_pair](src/iterator_pair.cc): ⭐⭐⭐ 通过接收 iterator_pair 来构造容器，以适配各类数据
    ```cpp
    template <class InputIterator>
    Container::Container(InputIterator begin, InputIterator end): size_(std::distance(begin, end)) { xxxx }
    ```
- [making_new_friends](src/making_new_friends.cc): ⭐⭐⭐ 在模板类中声明和定义友元操作符，解决在类外定义操作符时，操作符(比如 `operator<<` 不接受模板参数的问题)
- [member_detector](src/member_detector.cc): ⭐⭐⭐ 使用 SFINAE 来检测类是否含有名为 X 的成员(变量、函数、或类)
- [metafunction](src/metafunction.cc): ⭐⭐⭐ 通过元模板编程，在编译期确定类型或调用的函数(比如 `std::conditional_t`、[int_to_type](src/int_to_type.cc))
- [move_constructor](src/move_constructor.cc): ⭐ 在 c++11 以前，没有 move constructor 的情况下，通过隐式转换的方式来实现对临时变量的 move
    - 样例中的代码可以使用 gcc 5.1 --no-elide-constructors 来测试，测试平台可以使用 [godbolt](https://godbolt.org/)
- [multi_statement_macro](src/multi_statement_macro.cc): ⭐⭐⭐ 通过 `do {} while(0)` 来解决宏有多个语句带来的语法错误问题(并不能解决宏做参数的语法错误问题)
- [named_constructor](src/named_constructor.cc): ⭐⭐⭐  构造函数只能通过参数类进行区分，如果构造函数很多，那么在调用时就不便看出调用的哪一个构造函数。可以通过 static class function 来获得构造的对象，这样可以给 function 起一个 meaningful 的名字。
- [named_parameter](src/named_parameter.cc): ⭐⭐⭐ 在函数参数较多的情况下，可以定义一个 class 来封装这些参数，并设置一些函数来指定参数值，当代码更加清晰。比如在调用函数 `foo(int x, double y, float z, int a=1, double b=2, float c=3)`，不使用 named_parameter，就可能要写成 `foo(1, 2, 3, 1, 2, 4)` ，使用 named_parameter 就可以写成 `foo(Parameter().set_x(1).set_y(2).set_z(3).set_c(4))`，而且可以不用换 set_xx 的顺序
- [named_loop](src/named_loop.cc): ⭐ 通过在宏中使用 `goto` 来跳出代码块
- [nifty_counter](src/nifty_counter.cc): ⭐ 通过在 global static object 初始化和析构时使用计数器，对 global object 进行资源管理。不过感觉可以通过 static class function 中 static variable 来获取一个类似 global object 的实例(样例见 [construct_on_first_use](src/construct_on_first_use.cc))
- [non_copyable_mixin](src/non_copyable_mixin.cc): ⭐⭐⭐ 将拷贝构造和拷贝赋值操作符设置成 private，来避免类实例被拷贝和赋值
- [no_throwing_swap](src/non_throwing_swap.cc): ⭐⭐⭐⭐⭐ 通过交换两个实例中数据指针的方式来达到 no_throwing 的 swap
- ~~object_generator~~
- [non_virtual_interface](src/non_virtual_interface.cc): ⭐⭐ 通过 Base 类 public 接口，并在 public 接口中调用 private virtual function，以在接口中实现一些通过功能，并最终执行 Derived 类的逻辑
    ```cpp
    class Base {
    public:
        void say_hi() { std::println("hi"); say_hi_impl(); }
    private:
        virtual void say_hi_impl() = 0;
    };
    class Derive : public Base {
    private:
        void say_hi_impl() override { std::println("I'm instance of class 'Derived'"); }
    };
    ```
- [nullptr](src/nullptr.cc): ⭐ 在 c++11 以前，自己实现一个 `class nullptr_t` 替代 `NULL`
- [parameterized_base](src/parameterized_base.cc): ⭐⭐⭐⭐⭐ 通过将 Base 类作为模板参数，为某个类型 T 提供期望的接口能力，比如为类型 T 提供序列化能力 class ISerializable， `template <class T> Serializable: public T, public ISerializable {};`，和 [interface_class](src/interface_class.cc) 不同，这种方式可以在不改变类型 T 的定义下为其实现接口功能，特别是 T 是第三方库中的类型结构时。使用样例见 [construction_tracker](src/construction_tracker.cc) 中的 `Adapter<E> e_`
    > 有点像 rust traits，样例 [rust_traits_serializable](src/parameterized_base_deps/rust_traits_serializable/src/main.rs)
- [pointer_to_implementation](src/pointer_to_implementation.cc): ⭐⭐⭐ 将公开接口放到 class Public，并由 class Public 包含实现细节 class Public::Impl 的指针(class Public 的成员变量)，以此带来一些代码编译、解耦等方面的优势
- [policy_clone](src/policy_clone.cc): ⭐⭐⭐ 通过在模板类中定义一个同样的模板类，来方便使用者重新绑定模板参数(rebind)，比如输入的模板参数是 `Allocator<int>`，我们可以通过重新绑定模板参数的方式，将 `Allocator` 的模板参数从 `int` 变为 `Node<int>`，从而得到 `Allocator<Node<int>>`
    ```cpp
    template <class T>
    class Allocator { template <class U> using rebind = Allocator<U>; };
    template <class T>
    class Node {};
    template <class T, class Alloc=Allocator<T>>
    class List {
        using alloc = typename Alloc::template rebind<Node<T>>;
        // 这样，我们就可以在 List 中使用为 Node<T> 分配内存的分配器，而不是使用 T 的内存分配器
    }
    ```
- [polymorphic_exception](src/polymorphic_exception.cc): ⭐⭐⭐ 通过 `throw e;` 的方式来抛出异常，会将 e 进行拷贝构造，如果 e 是一个 Base 类的引用，其实际类型是 Derived 类，那么 `throw e;` 只会拷贝 Base 类的信息，而丢失 Derived 类的信息。可以借用虚函数，在虚函数中通过 `throw *this;` 方式来抛出异常，以避免 Derived 类的信息丢失
    ```cpp
    ExceptionDerived e;
    // no: throw e 会抛出 ExceptionBase 类型的异常，导致 ExceptionDerived 中的一些信息丢失
    void foo(ExceptionBase& e) { throw e; }
    // yes: ExceptionBase::raise() 是一个虚函数，通过 throw *this 来抛出 ExceptionDerived 类型的异常
    void foo2(ExceptionBase& e) { e.raise(); }
    ```
- [polymorphic_value_types](src/polymorphic_value_types.cc): **<span style="color:red">TODO</span>**
- requiring_or_prohibiting_head_based_objects: 限制某个类的实例只能在栈上构造，或者限制只能在堆上构造，同 [concrete_data_type](src/concrete_data_type.cc)
- [resource_acquisition_is_initialization](src/resource_acquisition_is_initialization.cc): ⭐⭐⭐⭐⭐ 通过将资源(指针)封装到资源管理类中，利用资源管理类的实例结束生命周期时，通过其析构函数来释放资源，比如通过  `std::unique_ptr<T>` 来 `delete t`，`std::lock_guard<T>` 来 `mutex.unlock`。另见 [execute_around_pointer](src/execute_around_pointer.cc)
- [resource_return](src/resource_return.cc): ⭐⭐⭐ 当函数返回裸指针指向新建的资源时(比如 `return new Foo`)，用户可能会忘记释放资源，此时，可以通过返回智能指针来管理和释放资源
- [return_type_resolver](src/return_type_resolver.cc): ⭐⭐⭐ 当函数返回类型取决于模板参数类型时，在调用函数时，需要将模板参数填入，同时承接返回的实例时也需要填写实例类型(如果模板参数类型和返回类型一致，其实可以用 auto 承接实例)。通过使用隐式转换，可以避免填写模板参数，函数返回类型取决于用何种类型承接。
- runtime_static_initialization_on_order: 通过 [construct_on_first_use](src/construct_on_first_use.cc) 和 [nifty_counter](src/nifty_counter.cc) 来解决 non-local static objects 初始化顺序的问题
- [safe_bool](src/safe_bool.cc): ⭐⭐⭐ 为类型 T 提供隐式转换操作符，将其转换成 bool 类型(`operator bool();`)，以用作条件判断，如 if (t), while(t) 等。但是转换成 bool 的方式有一些问题，需要使用一些方法来避免这些问题。
  - 问题如下:
    - 阻止相同类型的比较(`T t1, t2; if (t1 == t2) {}`)
    - 阻止不同类型的比较(`T1 t1; T2 t2; if (t1 == t2) {}`)
    - 阻止隐式转换成 bool 后又进行其他操作，如赋值、计算等(`int a = t;`, `int a = t + 1`)
  - 解决办法:
    - [boost explicit_operator_bool](https://www.boost.org/doc/libs/1_65_1/boost/core/explicit_operator_bool.hpp)
    - [boost safe_bool](https://www.boost.org/doc/libs/1_50_0/boost/spirit/home/classic/core/safe_bool.hpp)
    - c++11 explicit conversion operators: `explicit operator bool();`
- ~~scoped_guard~~
- [substitution_failure_is_not_an_error](src/substitution_failure_is_not_an_error.cc): ⭐⭐⭐⭐⭐ Substitution Failure Is Not An Error
- shrink_to_fit: ⭐⭐⭐ 将容器的 capacity 缩减成和 size 一样，以节省内存
    ```cpp
    std::vector<int> v{1, 2, 3, 4 ,5};
    std::vector<int>(v).swap(v);
    // 或者
    std::vector<int>(v.begin(), v.end()).swap(v);
    // 前面两种办法都会拷贝原有 v 的元素，最好是使用容器自带的 shrink_to_fit 函数，这样可能只在 v 原有的内存上进行缩减，而且当 capacity == size，什么也不用做。当然 shrink_to_fit 并不会严格的将 capacity 缩减到 size
    v.shrink_to_fit();
    ```
    - 另外，在 [clear_and_minimize](src/clear_and_minimize.cc) 中，也使用了 swap 方法来清理内存
- small_object_optimization: **<span style="color:red">TODO</span>** and SBO
- [smart_pointer](src/smart_pointer.cc): ⭐⭐⭐ 这个不是指 c++ STL 中的 std::shared_ptr、std::unique_ptr 等能用于资源管理的智能指针。在 [pointer_to_implementation](src/pointer_to_implementation.cc) 中，外部类可能需要定义多个和 class Impl 同名的成员函数，然后才能调用这些函数。在外部类通过定义 `Impl* operator()->()` 直接获取数据指针，以此调用其成员函数，减少代码量
    ```cpp
    struct Body;
    struct Handle {
        Body* operator->() { return body_; }
        const Body* operator->() const { return body_; }
        Body body_;
    };
    ```
- [tag_dispatching](src/tag_dispatching.cc): ⭐⭐⭐⭐⭐ 在函数参数中设置一个 tag class，用于区分不同的重载函数，调用这些函数时通过传入不同的 tag 来确定调用哪一个函数
    ```cpp
    struct Tag1 {};
    struct Tag2 {};
    void sort(Tag1, Container& container);
    void sort(Tag2, Container& container);
    template <class Tag>
    void call_sort(Tag t, Container& container) {
        sort(t, container);
    }
    ```
- [temporary_base_class](src/temporary_base_class.cc): ⭐⭐⭐⭐⭐ 在向量、矩阵、字符串等表达式计算中，会反复产生临时对象用于存储中间结果，构造和析构临时对象的过程会调用 new 和 delete 造成一些开销，这是可以通过一个临时的类来存储数据(new)，并将后续的计算结果存储到这个临时对象中，在表达式结果后再将数据所有权转移给计算结果的实例。
    ```cpp
    Matrix m1, m2, m3, m4;
    Matrix result = m1 + m2 + m3 + m4;
    // 以下为拆解的执行过程
    // Matrix tmp_1 = m1 + m2;  // new for tmp_1.data_
    // Matrix tmp_2 = tmp_1 + m3;  // new for tmp_2.data_, delete tmp_1.data_
    // Matrix tmp_3 = tmp_2 + m4;  // new for tmp_3.data_
    // Matrix result = tmp_3;  // result.data_ = tmp_3.dat_
    // 上面产生了三个临时对象，其中 tmp_1 通过 new 申请内存来存储 m1 + m2 的结果，然后在 tmp_1 + m3 计算结束后通过 delete 释放掉申请的内存
    // 同理，tmp_2 也是如此
    // 最后 tmp_3 通过编译器 rvo 将申请的内存资源即计算结果转移给 result

    // solution: 通过 TMatrix 存储计算结果
    // TMatrix tmp = m1 + m2;  // new for tmp.data_
    // tmp.add(m3);  // add m3.data_ to tmp.data_
    // tmp.add(m4);  // add m4.data_ to tmp.data_
    // Matrix result = tmp;  // result.data_ = tmp.data_
    // 这样只用一开始在 tmp 构造时申请内存，中间的计算结果全部存到 tmp 中，最终将申请的内存移交给 result
    ```
- [thin_template](src/thin_template.cc): ⭐⭐⭐ 通过将模板类中一些与模板参数无关的成员变量或函数提取到一个 Base 类中，以减少模板实例化后的目标代码大小。另外，共用同一份代码也可以提高缓存性能。比如将 vector 类中的 `size_t size_; size_t capacity_;` 放到 vector_base 类中，这样就不用在每个实例化的 vector<T> 代码中存一份相同的代码
- [thread_safe_copy_on_write](src/thread_safe_copy_on_write.cc): ⭐⭐⭐⭐⭐ 在多线程环境下更新一个共享的数据，通过将数据存储于 `std::atomic<std::shared_ptr<T>>` ，并使用 copy on write (其实这里应该是 copy and write)来保证读无锁、写安全。这里和前面的 idiom copy_on_write 不同，这里是为了 thread_safe 而使用了 copy，这里的 copy 是保证安全的一个手段，前面的 copy_on_write 的 copy 是目的。
    ```cpp
    // 拷贝所有数据到新的实例、写入新数据到新的实例、将新的实例的指针写入原指针中
    // std::atomic<std::shared_ptr<T>> data_ptr_;
    auto new_ptr = std::make_shared_ptr<T>(*data_ptr_.load());
    new_ptr->write_or_update(xxxx);
    data_ptr_.store(new_ptr);
    ```
- [type_generator](src/type_generator.cc): ⭐⭐⭐ 通过封装多参数模板类，并指定部分模板参数的默认值，来减少代码编写量，同时可以使用 `using` 关键字提供更多的便捷，比如 `template <bool B, class T, class F> std::condition_t =  std::condition<B, T, F>::type`
## Related Idioms
| idiom | related idioms | TODO(mark in code) |
| :------ | :-------------- | :------------: |
| algebraic_hierarch | handle_body, envelop_letter | no |
| capability_query | interface_class, inner_class | yes |
| coercion_by_member_template | generic_container_idioms | no |
| construct_on_first_use | nifty_schwarz_counter | no |
| copy_and_swap | non_throwing_swap, resource_acquisition_is_initialization | no |
