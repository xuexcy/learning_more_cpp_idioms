/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/21 22:30:16
# Desc    :
########################################################################
在某个操作之前和之后执行一些特定操作，就像 python 的 decorator

@solution: 将对象封装到特定操作的 class Aspect，通过构造 Aspect 的实例，利用构造和析构函数来执行特定操作
template <class T>
class Aspect {
public:
    Aspect(T t) : t_(t) {
        // t 操作前执行
    }
    T operator->() {
        return t_;
    }
    ~Aspect() {
        // t 操作后执行
    }
private:
    T t_;
};
template <class T>
class AspectWrapper {
public:
    AspectWrapper(T t): t_(t) {}
    Aspect operator->() {
        return Aspect(t_);
    }
private:
    T t_;
};

std::vector<int> vec;
AspectWrapper<std::vector<int>*> wrapper(&vec);
wrapper->push_back(3);

*/

#include <print>
#include <sys/proc.h>
#include <vector>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

namespace bad_case {
class Visualizer {
    std::vector<int>& vec_;
public:
    Visualizer(std::vector<int>& v): vec_(v) {}
    void data_changed() {
        std::println("Now size is: {}", vec_.size());
    }
};  // class Visualizer
void run() {
    std::println();
    PRINT_CURRENT_FUNCTION_NAME;
    std::vector<int> vec;
    Visualizer visu(vec);
    vec.push_back(10);
    // bad: 每次 vec 操作后，都要手动调用 visu.data_changed()
    visu.data_changed();
    vec.push_back(20);
    visu.data_changed();
}
}  // namespace bad_case

// Executed Around Proxy
namespace solution {
class VisualizableVector {
public:
    class Proxy {
    public:
        Proxy(std::vector<int>* v): vec_(v) {
            std::println("Before size is: {}", vec_->size());
        }
        std::vector<int>* operator->() {
            return vec_;
        }
        ~Proxy() {
            std::println("After size is: {}", vec_->size());
        }
    private:
        std::vector<int>* vec_;
    };  // class Proxy
    VisualizableVector(std::vector<int>* v): vec_(v) {}
    Proxy operator->() {
        return Proxy(vec_);
    }
private:
    std::vector<int>* vec_;
};  // class VisualizableVector

void run() {
    std::println();
    PRINT_CURRENT_FUNCTION_NAME;
    std::vector<int> vec;
    VisualizableVector visual_vec(&vec);
    visual_vec->push_back(10);
    visual_vec->push_back(20);
}
}  // namespace solution

namespace solution_2 {
// 这里通过 Aspect + 模板参数 NextAspect, 由 class AspectWeaver 来将各个 Aspect 连接起来
// 在使用 class AspectWeaver 时，首先执行第一个继承了 Aspect 的子类的构造函数，然后在 operator-> 被调用时又执行 NextAspect 的构造函数，以将多个 Aspect/decorator 串起来
template <class NextAspect, class Para>
class Aspect {
protected:
    Aspect(Para p): para_(p) {}
    Para para_;
public:
    NextAspect operator->() {
        return NextAspect(para_);
    }
};  // class Aspect

template <class Aspect, class Para>
class AspectWeaver {
public:
    AspectWeaver(Para p) : para_(p) {}
    Aspect operator->() {
        // 这个 Aspect 是 继承类，比如 Visualizing，构造时输出一些信息，然后 Aspect 的 operator->() 返回
        // NextAspect(para_)，继续在构造 NextAspect 时输出一些信息
        return Aspect(para_);
    }
private:
    Para para_;
};  // class AspectWeaver

template <class NextAspect, class Para>
struct Visualizing : Aspect<NextAspect, Para> {
public:
    Visualizing(Para p) : Aspect<NextAspect, Para>(p) {
        std::println("Before Visualization aspect");
    }
    ~Visualizing() {
        std::println("After Visualization aspect");
    }

};  // struct Visualizing

template <class NextAspect, class Para>
struct Locking : Aspect<NextAspect, Para> {
public:
    Locking(Para p) : Aspect<NextAspect, Para>(p) {
        std::println("Before Lock aspect");
    }
    ~Locking() {
        std::println("After Lock aspect");
    }
};  // struct Locking

template <class NextAspect, class Para>
struct Logging : Aspect<NextAspect, Para> {
public:
    Logging(Para p) : Aspect<NextAspect, Para>(p) {
        std::println("Before Logging aspect");
    }
    ~Logging() {
        std::println("After Logging aspect");
    }
};  // struct Logging

#define AW1(T, U) AspectWeaver<T<U, U>, U>
#define AW2(T, U, V) AspectWeaver<T<U<V,V>, V, V>
#define AW3(T, U, V, X) AspectWeaver<T<U<V<X, X>, X>, X>, X>

void run() {
    std::println();
    PRINT_CURRENT_FUNCTION_NAME;
    std::vector<int> vec;
    // AspectWeaver<Visualizing<Locking<Logging<std::vector<int> *, std::vector<int> *>, std::vector<int> *>, std::vector<int> *>, std::vector<int> *>
    AW3(Visualizing, Locking, Logging, std::vector<int>*) X(&vec);
    X->push_back(10);
    X->push_back(20);
}
}  // namespace solution_2


namespace solution_3 {
// 这里在 AspectWeaver 中使用变长模板的折叠表达式展开来将执行各个 Aspect 的构造函数，免得像 solution_2::AspectWeaver 那样写起来麻烦
template <class Para>
class Aspect {
protected:
    Aspect(Para p): para_(p) {}
    Para para_;
};  // class Aspect

template <class Para, template <class> class... Aspects>
struct AspectWeaver {
public:
    AspectWeaver(Para p): para_(p) {}
    Para operator->() {
        // 折叠表达式展开
        (Aspects<Para>(para_), ...);
        return para_;
    }
private:
    Para para_;
};  // struct AspectWeaver

template <class Para>
struct Visualizing : Aspect<Para>{
public:
    Visualizing(Para p) : Aspect<Para>(p) {
        std::println("Before Visualization aspect");
    }
    ~Visualizing() {
        std::println("After Visualization aspect");
    }

};  // struct Visualizing
template <class Para>
struct Locking : Aspect<Para> {
public:
    Locking(Para p) : Aspect<Para>(p) {
        std::println("Before Lock aspect");
    }
    ~Locking() {
        std::println("After Lock aspect");
    }
};  // struct Locking
template <class Para>
struct Logging : Aspect<Para> {
public:
    Logging(Para p) : Aspect<Para>(p) {
        std::println("Before Logging aspect");
    }
    ~Logging() {
        std::println("After Logging aspect");
    }
};  // struct Logging

template <class Para, template <class> class... Aspects>
struct AspectWeaverV2 {
public:
    AspectWeaverV2(Para p): para_(p) {}
    Para operator->() {
        (Aspects<Para>(para_), ...);
        return para_;
    }
private:
    Para para_;
};  // struct AspectWeaverV2

void run() {
    std::println();
    PRINT_CURRENT_FUNCTION_NAME;
    std::vector<int> vec;
    AspectWeaver<std::vector<int>*, Visualizing, Locking, Logging> X(&vec);
    X->push_back(10);
    X->push_back(20);
}

}  // namespace solution_3
int main() {
    utils::MainDecorator::Access();
    bad_case::run();
    solution::run();
    solution_2::run();
    solution_3::run();
    return 0;
}

