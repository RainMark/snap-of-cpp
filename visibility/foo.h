#pragma once

#include <string>

namespace test_1 {

extern int global;

class foo {
public:
    foo();

    int get();

private:
    int x_;

public:
    static std::string kMessage;
};

}
