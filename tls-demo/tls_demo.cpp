#include <tls_test.h>


extern __thread int* tls_ptr;

int run_test() {
    auto x = new int;
    set_tls_ptr(x);
    for (auto i = 0; i < 1024; ++i) {
        *get_tls_ptr() = i;
        *tls_ptr = 0;
    }
    return 0;
}
