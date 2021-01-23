
#include <stdint.h>
#include <vector>
#include <tls_test.h>

__thread int* tls_ptr = nullptr;

class foo {
public:
    int64_t a;
    std::vector<int> v;
};

thread_local foo f;


void set_tls_ptr(int* ptr) {
    tls_ptr = ptr;
}

int* get_tls_ptr() {
    f.a = reinterpret_cast<int64_t>(tls_ptr);
    return tls_ptr;
}
