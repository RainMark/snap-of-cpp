#ifndef Interface_H
#define Interface_H

#include <string>

class Interface {
public:
    Interface() {}
    ~Interface() {}

    virtual void println(const std::string& message) {
        std::cout << "base:" << message << std::endl;
    }
    virtual void println_c(const char* message, std::size_t size) {
        std::cout << "base:" << message << size << std::endl;
    }
};

#endif // Interface_H
