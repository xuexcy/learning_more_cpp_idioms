#pragma once

#include <print>


namespace utils {

class MainDecorator {
public:
    MainDecorator() {
        std::println("Start main");
    }
    ~MainDecorator() {
        std::println("Finished main\n");
    }
};

MainDecorator g_main_decorator;

}  // namespace utils
