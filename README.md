# [more_cpp_idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)

- [address_of](address_of.cpp): 在 `&` 操作符被重载的情况下，获取 object 的地址
- [algebraic_hierarchy](algebraic_hierarchy.cpp): 仅暴露抽象基类并在用户代码中进行使用, 如使用抽象基类`Number`表达数字, 定义继承类虚数`Complex`和实数`RealNumber`
    ```cpp
    // 如下代码中只使用了 Number 基类
    Number n1 = Number::makeComplex(1, 2);
    Number n2 = Number::makeRealNumber(5);
    Number n3 = n1 + n2;
    n3 = n2;
    ```
- [attach_by_initialization](src/attach_by_initialization.cc): 在用户代码 `main` 函数开始前，通过全局变量 global variable 执行一些工具类的初始化流程
- [attorney_client](src/attorney_client.cc): 通过代理类 `class Attorney` 控制友元类对私有成员的访问范围。`class A(friend class Attorney); class Attorney(friend class B)`
- [barton_nackman_trick](src/barton_nackman_trick.cc): 在模板类中将 operator 设置成 friend，解决 operator 重载带模板参数的问题
- [base_from_member](src/base_from_member.cc): 基类先于继承类成员构造，若基类构造依赖继承类的成员，构造结果将可能不符合预期。通过将基类依赖的成员提取到单独的 class Container, 再由继承类控制继承顺序, 来解决此类问题
- [boost_mutant](src/boost_mutant.cc): 对内存布局一致的两个 class，可以从同一个对象地址开始读取数据
- [calling_virtuals_during_initialization](src/calling_virtuals_during_initialization.cc): 通过two-phase-initialization,实现类似在 Base 类的构造函数中调用继承类的虚函数的逻辑
- [curiously_recurring_template_pattern](src/curiously_recurring_template_pattern.cc): 奇异递归模板模式, 基类模板参数为继承类，实现编译期多态，即在编译期确定在基类函数中调用哪个子类函数
- [interface_class](src/interface_class.cc): 在类中声明纯虚函数接口，实现接口类 class Interface

