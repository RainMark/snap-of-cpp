#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define SIZE 4096 * 4

int main(int argc, char* argv[]) {
    printf("%ld\n", sizeof(long));
    for (auto i = 0; i < 1024 * 1024; ++i) {
        char* foo = new char[SIZE];
        memset(foo, 0, 4096 * 2);
    }
    while (1);
}


template <class T>
Lazy<T> Foo() {
    // ...
    T val = co_await Bar();
    // ...
    co_return val;
}

template <class T>
Lazy<T> Bar() {
    // ...
}
