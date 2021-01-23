#include <iostream>
#include <dlfcn.h>
#include <common.h>

using namespace std;

int main(int argc, char* argv[]) {
    // open the library
    cout << "Opening foo.so...\n";
    // void* handle = dlopen("./foo.so", RTLD_LAZY);
    void* handle = dlopen("./foo.so", RTLD_NOW|RTLD_NODELETE);

    if (!handle) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }

    // load the symbol
    cout << "Loading symbol instance...\n";
    typedef Interface* (*instance_t)();
    // reset errors
    dlerror();
    auto instance = (instance_t) dlsym(handle, "instance");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'instance': " << dlsym_error << '\n';
        dlclose(handle);
        return 1;
    }

    // use it to do the calculation
    cout << "Calling instance...\n";
    auto println = instance();
    std::cout << println << std::endl;
    std::cout << "main abi version: " << _GLIBCXX_USE_CXX11_ABI << std::endl;
    std::string m("println()");
    // println->println(m);
    println->println_c(m.data(), m.length());

    // close the library
    cout << "Closing library...\n";
    dlclose(handle);
}
