#pragma once

#include <memory>
#include <string>

namespace test_1 {
class foo;
}

namespace test_2 {

class bar {
public:
    bar();

    int get();

private:
    std::unique_ptr<test_1::foo> f_;
};

}
