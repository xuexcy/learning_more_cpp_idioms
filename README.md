# [more_cpp_idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms)

- [address_of](address_of.cpp): 在 `&` 操作符被重载的情况下，获取 object 的地址
- algebraic_hierarchy: 仅暴露抽象基类并在用户代码中进行使用, 如使用抽象基类`Number`表达数字, 定义继承类虚数`Complex`和实数`RealNumber`
    ```cpp
    // 如下代码中只使用了 Number 基类
    Number n1 = Number::makeComplex(1, 2);
    Number n2 = Number::makeRealNumber(5);
    Number n3 = n1 + n2;
    n3 = n2;
    ```
- attach_by_initialization: 在用户代码 `main` 函数开始前，通过全局变量 global variable 执行一些工具类的初始化流程
- attorney_client: 通过代理类 `class Attorney` 控制友元类对私有成员的访问范围。`class A(friend class Attorney); class Attorney(friend class B)`

