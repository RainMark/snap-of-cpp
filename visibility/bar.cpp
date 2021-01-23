#include "bar.h"
#include "foo.h"

#define DLL_PUBLIC __attribute__ ((visibility ("default")))

namespace test_2 {

DLL_PUBLIC bar::bar() {
    f_ = std::make_unique<test_1::foo>();
}

DLL_PUBLIC int bar::get() {
    return f_->get();
}

}
