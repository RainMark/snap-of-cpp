#include <iostream>
#include "bar.h"
#include "foo2/foo2.h"

int main() {
    test_2::bar b;
    std::cout << b.get() << std::endl;
    test_1::foo f;
    std::cout << f.get() << std::endl;
    return 0;
}
