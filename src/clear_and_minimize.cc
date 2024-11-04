/**
容器分配的内存经常多于实际使用的内存，以优化 size 增大时的内存分配效率，但在 size 减少时也会浪费一些内存
在 size 减小时容器不会主动回收没有使用的内存, 比如 clear 之后，size 为 0，但 capacity 依然保持不变

@solution:
通过空的 container 和当前 container 进行 swap 来回收内存, 这样空的 container 因为不再使用所占用的
内存被程序收回，当前 container 因为 swap 后，size = 0, capacity = 0
std::vector<T>().swap(v)
 */

#include <print>
#include <vector>

#include "utils/main_decorator.h"

template <class T>
void print_container(const std::string& phase, const T& container) {
    std::println(
        "phase after: {}, size: {}, capacity: {}",
        phase, container.size(), container.capacity());
}

/**
@stdout:
Start main
phase after: init, size: 10000, capacity: 10000
phase after: push, size: 10001, capacity: 20000
phase after: clear, size: 0, capacity: 20000
phase after: push, size: 1, capacity: 20000
phase after: clear by swap with empty, size: 0, capacity: 0
Finished main
*/

int main() {
    utils::MainDecorator::Access();
    std::vector<int> v(10000, 1);
    print_container("init", v);
    v.emplace_back(1);
    print_container("push", v);
    v.clear();
    print_container("clear", v);
    v.push_back(1);
    print_container("push", v);
    decltype(v)().swap(v);
    print_container("clear by swap with empty", v);
    return 0;
}
