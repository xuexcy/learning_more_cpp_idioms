#include "message_queue.h"

#include <print>


// 需要放到 namespace 之外才能在 main 前初始化
message_queue::MessageQueue global_msg_queue;

namespace message_queue {

uint64_t get_uuid() {
    return 123455;
}

MessageQueue::MessageQueue():
    address_("127.0.0.1:9092"),
    cache_size_(100'000),
    uuid_(get_uuid()) {
    std::print(
        "Initialize MessageQueue finished! local uuid: {}, message_queue address: {}\n",
        uuid_, address_);
}

}  // namespace message_queue
