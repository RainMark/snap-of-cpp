#include <iostream>
#include "minico.h"

minico_function(bar, int, x)
    std::cout << "bar sleep " << x << "ms" << std::endl;
    minico_await(bar, 1, minico::sleep_ms(this, x));
    std::cout << "bar after sleep " << x << "ms" << std::endl;
    minico_return(x);
};

minico_function(foo, int, x)
    std::cout << "foo sleep " << x << "ms" << std::endl;
    minico_await(foo, 1, minico::sleep_ms(this, x));
    std::cout << "foo after sleep " << x << "ms" << std::endl;
    minico_call(bar(100));
    std::cout << "after call bar" << std::endl;
    minico_return(x)
};


int main(int argc, char* argv[]) {
    minico_call(foo(300));
    std::cout << "hahaha" << std::endl;
    minico_exit();
}
