#include <iostream>
#include <type_traits>
#include <functional>
#include <tuple>
#include <thread>

template <class T>
class TypeWrapper {
public:
    using ValueType = T;

    T val;
};


TypeWrapper<int> foo(double* x) {
    TypeWrapper<int> tw;
    tw.val = int(*x);
    return tw;
}

void bar(int, int) {
}

template <class Coroutine, class... Args>
typename std::result_of_t<std::decay_t<Coroutine>(std::decay_t<Args>...)>::ValueType
await(Coroutine&& coroutine, Args&&... args) {
    typename std::result_of_t<std::decay_t<Coroutine>(std::decay_t<Args>...)>::ValueType val;
    auto targs = std::forward_as_tuple(std::forward<Args>(args)...);
    std::thread([&val, coroutine = std::move(coroutine), targs = std::move(targs)]() mutable {
                    val = std::apply(coroutine, targs).val;;
                    std::cout << val << std::endl;
                }).detach();
    return val;
}

template <class F, class... Ts>
constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
arguments_of(F(*f)(Ts...)) {
    return std::integral_constant<std::size_t, sizeof...(Ts)>{};
}

template <class F, class C, class... Ts>
constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
arguments_of(F(C::*f)(Ts...)) {
    return std::integral_constant<std::size_t, sizeof...(Ts)>{};
}

template <class R, class C, class M, class... Args>
void match() {
}

template <class Callable>
void call() {
    match<decltype(&Callable::operator())>();
}

class foo_class {
public:
    foo_class() = default;
    ~foo_class() = default;

    void foo(int, double, int) {}
};

int main() {
    double val = 1.2;
    std::cout << await(foo, &val) << std::endl;
    std::cout << decltype(arguments_of(bar))::value << std::endl;
    foo_class fc;
    // std::cout << decltype(arguments_of(fc.foo))::value << std::endl;
    auto lm = [](int, int) mutable -> int {
                  return 0;
              };
    // std::cout << decltype(&lm::operator()) << std::endl;
    call<decltype(lm)>();
    return 0;
}
