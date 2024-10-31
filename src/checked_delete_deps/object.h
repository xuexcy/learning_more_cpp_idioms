#pragma once

#include <print>

#include "cpp_utils/util.h"

class Object {
public:
    ~Object() {
        std::println("{}", CURRENT_FUNCTION_NAME);
    }
};
