#pragma once

#include <cstddef>
#include <queue>
#include <print>
#include <string>

namespace message_queue {

class MessageQueue {
public:
    MessageQueue();
    template <class Msg>
    bool send(const std::string& field, const Msg& msg);
private:
    std::string address_;
    size_t cache_size_{0};
    std::queue<std::string> queue_;
    uint64_t uuid_{0};
};


template <class Msg>
bool MessageQueue::send(const std::string& field, const Msg& msg) {
    // queue_.push_back(to_string(msg));

    std::print("Send to message_queue success, key: {}, msg: {}\n", field, msg);
    return true;
}

}  // namespace message_queue
extern message_queue::MessageQueue global_msg_queue;
#define MESSAGE_QUEUE_SEND(field, msg) global_msg_queue.send(field, msg);
