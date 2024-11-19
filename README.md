# [More C++ Idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)
## Build And Run
- `env_variable.sh`: 设置环境变量，如 cmake 工作目录、可执行文件存放的 bin 文件夹目录等
- `build.sh`: 编译所有 src/*.cc
- `run.sh`: 运行所有编译后的可执行文件，并将结果输出到 src 目录
- `build_one.sh`: 编译 src 目录下的一个 .cc 文件，具体编译哪一个文件取决于 `env_variable.sh` 中的 `default_execute_file` 环境变量, 这个脚本主要是自己修改、测试某个 .cc 代码的时候使用，该文件里没有调用 cmake，如果编译的相关依赖发生改变，请重新执行 `build.sh` 生成新的 makefile 文件
- `run.sh`: 执行一个编译结果，同 `build_one.sh` 一样，具体哪个取决于 `env_variable.sh` 中的 `default_execute_file`
- 其他:
  - `run.sh` 执行后的输出结果在 src 目录下（可以通过 `env_variable.sh` 进行配置）
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
- [inner_class](src/inner_class.cc): ⭐⭐ 通过在 Derived 中定义不同的 Inner 类来继承不同的 Base，以解决在不同 Base 间含有同名虚函数时，为不同 Base 各自实现虚函数的问题
    > 在 rust 中没有继承，接口能力通过 traits 赋予各个 struct，这样可以在不同的 impl Trait for Struct 中实现同名函数，样例 [rust_traits_same_function_name](src/inner_class_deps/rust_traits_same_function_name/src/main.rs)
- [interface_class](src/interface_class.cc): ⭐⭐⭐⭐⭐ 在类中声明纯虚函数接口，实现接口类 class Interface
- [no_throwing_swap](src/non_throwing_swap.cc): ⭐⭐⭐⭐⭐ 通过交换两个实例中数据指针的方式来达到 no_throwing 的 swap
- [parameterized_base](src/parameterized_base.cc): ⭐⭐⭐⭐⭐ 通过将 Base 类作为模板参数，为某个类型 T 提供期望的接口能力，比如为类型 T 提供序列化能力 class ISerializable， `template <class T> Serializable: public T, public ISerializable {};`，和 [interface_class](src/interface_class.cc) 不同，这种方式可以在不改变类型 T 的定义下为其实现接口功能，特别是 T 是第三方库中的类型结构时。使用样例见 [construction_tracker](src/construction_tracker.cc) 中的 `Adapter<E> e_`
    > 有点像 rust traits，样例 [rust_traits_serializable](src/parameterized_base_deps/rust_traits_serializable/src/main.rs)
- [pointer_to_implementation](src/pointer_to_implementation.cc): ⭐⭐⭐ 将公开接口放到 class Public，并由 class Public 包含实现细节 class Public::Impl 的指针(class Public 的成员变量)，以此带来一些代码编译、解耦等方面的优势
- [SFINAE](src/SFINAE.cc): ⭐⭐⭐⭐⭐ Substitution Failure Is Not An Error
- [thread_safe_copy_on_write](src/thread_safe_copy_on_write.cc): ⭐⭐⭐⭐⭐ 在多线程环境下更新一个共享的数据，通过将数据存储于 `std::atomic<std::shared_ptr<T>>` ，并使用 copy on write (其实这里应该是 copy and write)来保证读无锁、写安全。这里和前面的 idiom copy_on_write 不同，这里是为了 thread_safe 而使用了 copy，这里的 copy 是保证安全的一个手段，前面的 copy_on_write 的 copy 是目的。
    ```cpp
    // 拷贝所有数据到新的实例、写入新数据到新的实例、将新的实例的指针写入原指针中
    // std::atomic<std::shared_ptr<T>> data_ptr_;
    auto new_ptr = std::make_shared_ptr<T>(*data_ptr_.load());
    new_ptr->write_or_update(xxxx);
    data_ptr_.store(new_ptr);
    ```
## Related Idioms
| idiom | related idioms | TODO(mark in code) |
| :------ | :-------------- | :------------: |
| algebraic_hierarch | handle_body, envelop_letter | no |
| capability_query | interface_class, inner_class | yes |
| coercion_by_member_template | generic_container_idioms | no |
| construct_on_first_use | nifty_schwarz_counter | no |
| copy_and_swap | non_throwing_swap, resource_acquisition_is_initialization | no |
