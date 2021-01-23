#pragma once
#include <atomic>
#include <queue>
#include <future>
#include <unordered_map>

namespace minico {

class MinicoLoop;
class MinicoFunction {
 public:
    virtual bool done() = 0;
    virtual bool ready() = 0;
    virtual void resume() = 0;
    virtual void set_ready() = 0;
    virtual void reset_ready() = 0;
    virtual ~MinicoFunction() {}
};

class MinicoLoop {
 public:
    MinicoLoop() {}
    ~MinicoLoop() {
        for (auto& it : _co_bodys) {
            delete it.first;
        }
    }

 public:
    std::vector<std::function<void()>>& function_bodys(MinicoFunction *func) {
        return _co_bodys[func]; }
    void register_function_body(MinicoFunction *func, std::function<void()> body) {
        auto it = _co_bodys.find(func);
        if (it != _co_bodys.end()) {
            it->second.push_back(body);
        } else {
            std::vector<std::function<void()>> bodys({body});
            _co_bodys.emplace(func, std::move(bodys));
        }
    }

    void shutdown() {
        _keep_run = false; }
    void submit(MinicoFunction* func) {
        _wait_queue.push(func); }

    void start_loop() {
        for (;_keep_run;) {
            while (!_wait_queue.empty()) {
                auto func = _wait_queue.front(); _wait_queue.pop();
                if (func->ready()) {
                    _ready_queue.push(func);
                } else {
                    _wait_queue.push(func);
                }
            }
            while (!_ready_queue.empty()) {
                auto func = _ready_queue.front(); _ready_queue.pop();
                func->resume();
                if (!func->done()) {
                    _wait_queue.push(func);
                }
            }
            if (_wait_queue.empty()) {
                return;
            }
        }
    }

 private:
    std::unordered_map<MinicoFunction *, std::vector<std::function<void()>>> _co_bodys;
    std::queue<MinicoFunction *> _ready_queue;
    std::queue<MinicoFunction *> _wait_queue;
    bool _keep_run = true;
};

void sleep_ms(MinicoFunction* caller, int32_t timeout) {
    std::thread([caller](int32_t timeout) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                    caller->set_ready();
                }, timeout).detach();
}

} // namespace minico

#define minico_init()                           \
    minico::MinicoLoop __minico_loop__

#define minico_exit()                           \
    __minico_loop__.start_loop();               \
    return 0

// #define minico_has_member(member)                                       \
//     template<class T, class... Args>                                    \
//     struct minico_has_member_##member {                                 \
//         template<calss U>                                               \
//         static auto check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
//         template<class U>                                               \
//         static auto check(...) -> decltype(std::false_type());          \
//         static const bool value = std::is_same<decltype(check<T>(0)), std::true_type>::value; \
//     };

#define minico_function(function_symbol, argument_type, argument_symbol) \
    class function_symbol : minico::MinicoFunction {                    \
    public:                                                             \
    ~function_symbol() override {}                                      \
    function_symbol(argument_type argument_symbol)                      \
        : argument_symbol(argument_symbol) {                            \
        __minico_loop__.register_function_body(                         \
            this, [this](){ this->function_symbol##_body_1(); });       \
        __minico_loop__.submit(this);                                   \
        reset_ready();                                                  \
        function_symbol##_body_0();                                     \
    }                                                                   \
    void resume() override {                                            \
        auto& bodys = __minico_loop__.function_bodys(this);             \
        bodys[_body_addr++]();                                          \
        if (_body_addr >= bodys.size()) {                               \
            _done = true;                                               \
        }                                                               \
        reset_ready();                                                  \
    }                                                                   \
    bool done() override {                                              \
        return _done; }                                                 \
    bool ready() override {                                             \
        return _ready.load(std::memory_order_acquire); }                \
    void set_ready() override {                                         \
        _ready.store(true, std::memory_order_release); }                \
    void reset_ready() override {                                       \
        _ready.store(false, std::memory_order_release); }               \
    argument_type argument_symbol;                                      \
    std::atomic<bool> _ready;                                           \
    int32_t _body_addr = 0;                                             \
    bool _done = false;                                                 \
    void function_symbol##_body_0() {

#define minico_await(function_symbol, point, ...)   \
        __VA_ARGS__;                                \
    }                                               \
    void function_symbol##_body_##point() {

#define minico_return(return_expr) }

#define minico_call(...) new __VA_ARGS__

minico_init();
