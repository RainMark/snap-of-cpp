#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <assert.h>

std::atomic<bool> data(false);
std::atomic<bool> ready(false);

void notify() {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    data.store(true, std::memory_order_relaxed);
    ready.store(true, std::memory_order_relaxed);
    // ready.store(true, std::memory_order_release);
}

void bench() {
    std::thread(notify).detach();
    while (!ready.load(std::memory_order_relaxed));
    // while (!ready.load(std::memory_order_acquire));
    assert(data.load(std::memory_order_relaxed));
    // ready.store(false, std::memory_order_relaxed);
}

int main(int argc, char *argv[]) {
    bench();
    return 0;
}
