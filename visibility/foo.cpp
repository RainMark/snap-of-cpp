#include "foo.h"

namespace test_1 {

std::string foo::kMessage = "xxx";

int global = 0;

foo::foo() {
    x_ = 0;
}

int foo::get() {
    for (auto i = 0; i < 10; ++i) {
        x_++;
    }
    return x_;
}

}
