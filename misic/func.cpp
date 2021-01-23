#include <iostream>
#include <functional>
#include <thread>
#include <algorithm>

using Func = std::function<void()>;

template <class F>
class lambda_task {
    F func_;

public:
    lambda_task(const F& func) : func_(func) {}
    lambda_task(F&& func) : func_(std::move(func)) {}
    void run_impl() noexcept { func_(); }
};

template <class F>
std::thread run_async(F&& f) {
    auto task = std::make_unique<lambda_task<F>>(std::forward<F>(f));
    return std::thread(
        [task = std::move(task)]() {
            task->run_impl();
        });
}

std::thread schedule(Func func) {
    return run_async(
        [func = std::move(func)]() mutable {
            func();
        });
}

int main() {
    int x = 0;
    auto t = schedule(
        [x]() {
            std::cout << x << std::endl;
        });
    if (t.joinable()) {
        t.join();
    }
    return 0;
}
