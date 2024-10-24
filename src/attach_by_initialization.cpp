/**
在不改变main函数的情况下(其他人/其他框架负责编写main逻辑)，向框架内添加一些初始化逻辑。

比如我写了一个用于数据采集的消息队列 class MessageQueue 库，用户可以通过
SEND_MSG("score", 1);
SEND_MSG("name", "bob");
等方式来采集数据。
在使用 MSG_SEND 前可能要做一些初始化操作，比如确定发送的地址，本地缓存大小等。
方法一: class MessageQueue 提供初始化接口，由编写 main 的开发主动调用, 比如
int main() {
    MessageQueue::Init();
    xxx
    xxx
}
方法二: 如果没有办法要求 main 的开发人员添加初始化操作，那么可以在 message_queue.cpp 中，
添加一个全局变量，这样全局变量会在 main 开始前进行初始化操作

*/

#include <cassert>
#include <print>

#include "attach_by_initialization_deps/server.h"
#include "utils/main_decorator.h"

int main() {
    std::print("Start server\n");
    Server server;
    assert(true == server.Start() && "Start server failed\n");
    std::print("Quit server");
    return 0;
}
