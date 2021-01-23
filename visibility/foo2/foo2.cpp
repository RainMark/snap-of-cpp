#include "foo2.h"

namespace test_1 {

std::string foo::kMessage = "xxx";

int global = 0;

foo::foo() {
    x_ = 100;
}

int foo::get() {
    return x_;
}

}
