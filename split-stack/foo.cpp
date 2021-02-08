#include "foo.h"

int foo(int n) {
    char b[1024 * 1024 * 200] = {0};
    return n + 1;
}
