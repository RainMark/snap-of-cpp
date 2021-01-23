#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <assert.h>

template <class T>
class stack {
public:
    stack() : head_(nullptr) {}
    ~stack() {}

public:
    struct cell {
        T element_;
        cell* next_ = nullptr;
    };

    void push(cell* c) {
        auto head = head_.load(std::memory_order_relaxed);
        do {
            c->next_ = head;
        } while (!head_.compare_exchange_weak(head,
                                              c,
                                              std::memory_order_relaxed,
                                              std::memory_order_relaxed));
    }

    cell* pop() {
        cell* next = nullptr;
        auto head = head_.load(std::memory_order_relaxed);
        do {
            if (nullptr == head) {
                return nullptr;
            }
            next = head->next_;
        } while (!head_.compare_exchange_weak(head,
                                              next,
                                              std::memory_order_relaxed,
                                              std::memory_order_relaxed));
        head->next_ = nullptr;
        return head;
    }

private:
    std::atomic<cell*> head_;
};

int main(int argc, char *argv[]) {
    stack<int> s;
    std::thread t1(
        [&s]() {
            for (auto i = 0; i < 100; ++i) {
                auto c = new stack<int>::cell;
                c->element_ = i;
                s.push(c);
            }
        });

    std::thread t2(
        [&s]() {
            for (auto i = 0; i < 100; ++i) {
                auto c = new stack<int>::cell;
                c->element_ = i;
                s.push(c);
            }
        });

    std::thread t3(
        [&s]() {
            for (auto i = 0; i < 100; ++i) {
                auto c = new stack<int>::cell;
                c->element_ = i;
                s.push(c);
            }
        });

    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();

    auto length = 1;
    for (auto c = s.pop(); c; ++length) {
        assert(c->element_ < 100);
        c = s.pop();
        if (!c) {
            break;
        }
    }
    std::cout << length << std::endl;

    return 0;
}
