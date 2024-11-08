# [more_cpp_idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)

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
- [curiously_recurring_template_pattern](src/curiously_recurring_template_pattern.cc): ⭐⭐⭐⭐ 奇异递归模板模式， 基类模板参数为继承类，实现编译期多态，即在编译期确定在基类函数中调用哪个子类函数
- [interface_class](src/interface_class.cc): ⭐⭐⭐⭐⭐ 在类中声明纯虚函数接口，实现接口类 class Interface
- [parameterized_base](src/parameterized_base.cc): ⭐⭐⭐⭐⭐ 通过将 base 类作为模板参数，为某个类型 T 提供期望的接口能力，比如为类型 T 提供序列化能力 class ISerializable， `template <class T> Serializable: public T, public ISerializable {};`，和 [interface_class](src/interface_class.cc) 不同，这种方式可以在不改变类型 T 的定义下为其实现接口功能，特别是 T 是第三方库中的类型结构时。使用样例见 [construction_tracker](src/construction_tracker.cc) 中的 `Adapter<E> e_`
    > 有点像 rust traits，样例 [rust_traits_serializable](src/parameterized_base_deps/rust_traits_serializable/src/main.rs)


# TODO
- [x] construction_tracker 中的 adapter 怎么实现
- [x] 括号操作符
- [ ] parameterized_base: rust traits

# Note
- xx
- xx
