#include <type_traits>
#include <iostream>

auto func2(char) -> int (*)()
{
    return nullptr;
}

int foo(const int&) {
    return 0;
}

class bar {
public:
    void ok(int) {}

    void run() {
        std::cout << std::is_invocable<decltype(&bar::ok), bar, int>::value << std::endl;
    }
};

// template <class F, class... Ts>
// constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
// arguments_of(F(*f)(Ts...)) {
//     return std::integral_constant<std::size_t, sizeof...(Ts)>{};
// }

// template <class R, class C, class... Ts>
// struct lambda : std::integral_constant<std::size_t, sizeof...(Ts)> {};

// template <class R, class C, class... Ts>
// struct lambda_type<R(C::*)(Ts...)> : lambda<R, C, Ts...> {};


// template <class L>
// constexpr std::integral_constant<std::size_t, lambda_type<decltype(&L::operator())>::value>
// arguments_of(L) {
//     return std::integral_constant<std::size_t, lambda_type<decltype(&L::operator())>::value>{};
// }

// template <class F, class... Ts>
// constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
// arguments_of(F(*)(Ts...)) {
//     return std::integral_constant<std::size_t, sizeof...(Ts)>{};
// }

// template <class F, class... Ts>
// constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
// arguments_of(F(*f)(Ts...)) {
//     return std::integral_constant<std::size_t, sizeof...(Ts)>{};
// }

// template <class R, class F, class First, class... Ts>
// constexpr std::bool_constant<std::is_same<std::decay_t<First>, Promise<R>>::value>
// first_is_promise(F(*f)(First, Ts...)) {
//     return std::bool_constant<std::is_same<std::decay_t<First>, Promise<R>>::value>{};
// }

template <typename T, class R, typename... U>
using FuncType = decltype(static_cast<R(T::*)(U...)>(&std::decay<T>::type::operator()));

int main()
{
    static_assert( std::is_invocable<int()>::value );
    static_assert( std::is_invocable_r<int, int()>::value );
    static_assert( std::is_invocable_r<void, void(int), int>::value );
    static_assert( std::is_invocable_r<int(*)(), decltype(func2), char>::value );

    auto lm = [](int) mutable -> int {
                  return 0;
              };
    std::cout << std::is_invocable<decltype(lm), int>::value << std::endl;
    std::cout << std::is_invocable<decltype(foo), int>::value << std::endl;

    bar b;
    std::cout << std::is_invocable<decltype(&bar::ok), bar, int>::value << std::endl;
    std::cout << std::is_invocable<decltype(&decltype(lm)::operator()), decltype(lm), int>::value << std::endl;
    b.run();

    // std::cout << arguments_of<foo>::value << std::endl;
    // std::cout << arguments_of<lm>::value << std::endl;
    // decltype(&decltype(lm)::operator()) t;
    // std::cout << typeid(t).name() << std::endl;
    FuncType<decltype(lm), int, int> t;
    std::cout << typeid(t).name() << std::endl;
    std::cout << std::is_class<decltype(lm)>::value << std::endl;
    std::cout << std::is_class<decltype(foo)>::value << std::endl;

    return 0;
}
