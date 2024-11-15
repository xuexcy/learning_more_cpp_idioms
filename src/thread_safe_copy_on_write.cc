/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/12 19:17:50
# Desc    :
########################################################################

在 copy_on_write.cc 的 solution::COWBase 中，成员变量 std::shared_ptr<T> m_ptr 并不是原子的，在多线程
情况下修改 m_ptr 是不安全的，比如在函数 clone_if_need 中对 m_ptr 赋值
    m_ptr = std::make_shared<T>(*old)
为此，如果需要在多线程情况下使用 copy on write, 那么就需要保证在对 m_ptr 的写入是安全的，那么我们可以将 m_ptr 设置
成原子的(需要c++20支持 std::atomic<std::shared_ptr<U>>)

另外，说一下 ThreadSafeCowPtr 和 copy_on_write.cc 中 COWBase 的不同
1. COWBase: 这个 class 是在 write 前 copy 出一个新的实例并使用，也就是一分为二，比如样例中的 COWBook，一开始是大家共享 Book
用于读数据(std::string solution::Book::string() const)，然后在 write 前将书一分为二以便新实例 write 的数据不影响其他的实例，
这样 copy 后的数据就可以随意更新 title/content/note 等信息
2. ThreadSafeCowPtr: 这个 class 存储了一个 map 用于更新 key-value 配置，多个线程共享 ThreadSafeCowPtr 的一个实例，
然后某个线程更新数据后，其他线程都读到新的数据。下面有两种方法实现多线程读写：
    a. 在读写前都加锁，然后直接读写数据，但是如果多读少写，那么读加锁就有些影响性能
    b. ThreadSafeCowPtr：所有的线程都在读同一份数据，如果有线程想要更新数据，那就把旧数据 copy 一份，然后在这个 copy 后的数据上进行
        update，再将共享数据的指针指向这个更新后的数据

COWBase 的目的本就是 copy，只是通过 copy on write 来达到 lazy copy 以优化性能。
ThreadSafeCowPtr 的目的并不是为了 copy，而是为了在更新多线程间共享的数据，通过 copy 的方式来实现 thread safe。

前者是为了 copy and then write，后则是为了 thread safe write 而使用了 copy （也就是说不用 copy 也可以达到相同的目的，比如读写加锁）
当然，也可以通过 ThreadSafeCowPtr 中的实现方式做到 ThreadSafeCOWBase，即 thread_safe + lazy_copy。
*/


#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <print>
#include <random>
#include <thread>
// #include "folly/concurrency/AtomicSharedPtr.h"

// #include "cpp_utils/util.h"

// #include "utils/main_decorator.h"

using namespace std::chrono_literals;

template <typename KeyT, typename ValT>
class ThreadSafeCowPtr {
public:
    using DataT = std::map<KeyT, ValT>;
    ThreadSafeCowPtr(): data{std::make_shared<DataT>()} {
        // assert(data.is_lock_free());
    }
    std::shared_ptr<DataT> read() {
        return data.load();
        // return std::atomic_load(&data);
    }
    void write(std::shared_ptr<DataT> new_ptr) {
        data.store(new_ptr);
        // std::atomic_store(&data, new_ptr);
    }
    void update(const KeyT& key, const ValT& val) {
        // 加锁是为了方式多个线程同时 update 导致数据丢失
        // 假设不加锁且 map 大小为 2，此时两个线程同时读 data，读到大小为 2 的 map
        // 然后都在各自的线程的 new_ptr 中 emplace，那就各自得到一个大小为 3 的 map
        // 然后再 this->write(new_ptr)，最终 data 更新成其中一个线程中的 map，大小为 3
        // 也就是说每个线程必须等待上一个线程 update 完成后才能 update
        std::lock_guard<std::mutex> lk(update_mut);
        auto new_ptr = std::make_shared<DataT>(*this->read());
        new_ptr->insert_or_assign(key, val);
        ++write_seq_num;
        std::println("    \033[31mWRITE in update, write_seq_num: {}, key: {}, value: {}\033[0m", write_seq_num.load(), key, val);
        this->write(new_ptr);
    }
private:
    std::mutex update_mut;
    // folly::atomic_shared_ptr<DataT> data;

    // std::atomic<std::shared_ptr<T>>
    // P0718R2: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0718r2.html
    // 编译器支持进度: https://en.cppreference.com/w/cpp/compiler_support#C.2B.2B20_library_features
    // https://en.cppreference.com/w/cpp/atomic/atomic
    // https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic2

    // 目前(2024/11/14) clang 19.1.3 都没有支持 std::atomic<std::shared_ptr>
    // clang cxx20: https://libcxx.llvm.org/Status/Cxx20.html
    // 吃瓜: https://us.v2ex.com/t/1081936

    std::atomic<std::shared_ptr<DataT>> data;
    // std::shared_ptr<DataT> data;
    std::atomic_uint8_t write_seq_num{0};
};


std::random_device rd;
std::mt19937 gen(rd());
const uint8_t MAX_NAME_N{26};
const uint8_t MAX_SCORE_N{10};
std::uniform_int_distribution<uint8_t> dis_name(0, MAX_NAME_N - 1);
std::uniform_int_distribution<uint8_t> dis_score(0, MAX_SCORE_N - 1);
using Name = char;
using Score = int;
using Data = std::map<Name, Score>;
using Ptr = ThreadSafeCowPtr<Name, Score>;
void print(int id, int epoch, const Data& data) {
    static const std::string init_str{""};
    std::println("READ, thread_id: {}, epoch: {}, name_to_score: {{ {} }}",
        id, epoch,  std::accumulate(
            data.begin(), data.end(), init_str,
            [](auto pre, auto& cur) {
            auto cur_str = std::format("[{}:{}]", cur.first, cur.second);
            return pre.empty() ? cur_str : std::format("{}, {}", pre, cur_str);
            }
        )
    );

}
void task(int id, int epoch, int& write_cnt, ThreadSafeCowPtr<Name, Score>& ptr) {
    bool write{false};
    // 3/MAX_SCORE_N 的概率更新 score
    if (dis_score(gen) < 3) {
        // 'A' - 'Z'
        char name = 'A' + dis_name(gen);
        // [0, 9]
        auto score = dis_score(gen);
        ++write_cnt;
        std::println(
            "    \033[32mWRITE_{}, thread_id: {}, epoch: {}, name: {}, score: {}\033[0m",
            write_cnt, id, epoch, name, score);
        ptr.update(name, score);
    } else {
        print(id, epoch, *ptr.read());
    }
    std::this_thread::sleep_for(1ms);
}

void run_thread_safe_cow_ptr() {
    std::vector<std::thread> threads;
    ThreadSafeCowPtr<Name, Score> name_to_score;
    constexpr uint8_t thread_num{5};
    constexpr uint8_t epoch{15};
    for (auto i = 0; i < thread_num; ++i) {
        threads.emplace_back([i, &name_to_score]() {
            int write_cnt{0};
            for (int j = 0; j < epoch; ++j) {
                task(i, j, write_cnt, name_to_score);
            }
            std::println("thread_id: {}, READ count: {}, WRITE count: {}", i, epoch - write_cnt, write_cnt);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::println("\nThreads joined!!!\n");
    print(thread_num, epoch, *name_to_score.read());
}
int main() {
//    utils::MainDecorator::Access();
    run_thread_safe_cow_ptr();
    return 0;
}
