#include <iostream>
#include <string>
#include <common.h>

#ifndef _GLIBCXX_USE_CXX11_ABI
#define _GLIBCXX_USE_CXX11_ABI 0
#endif

class Println : public Interface {
public:
    Println() {}
    ~Println() {}

    void println(const std::string& message) override {
        std::cout << message.length() << message << std::endl;
    }

    void println_c(const char* message, std::size_t size) override {
        std::cout << "so abi version: " << _GLIBCXX_USE_CXX11_ABI << std::endl;
        std::cout << this << std::endl;
        std::string m(message, size);
        println(m);
    }
};

extern "C"
Interface* instance() {
    static Println p;
    return &p;
}
