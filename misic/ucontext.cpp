#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define STACK_SIZE 4096
#define offset 128

#define SEGV_STACK_SIZE STACK_SIZE * 10

size_t page_size = 4096;

char* stack;
ucontext_t m, f;

#define crash(fmt, args...)                     \
    do {                                        \
        printf(fmt, ##args);                    \
        exit(-1);                               \
    } while (0)

void show_satck() {
    for (auto i = 0; i < STACK_SIZE + offset; ++i) {
        if (stack[i] != 'p' && stack[i] != 'a') {
            putchar('x');
        } else {
            putchar(stack[i]);
        }
    }
    puts("\n");
}

void sigsegv_handler(int signum, siginfo_t *info, void *data) {
    void *addr = info->si_addr;

    mprotect(stack, page_size, PROT_READ|PROT_WRITE);
    printf("core\n");
    // show_satck();

    // crash("address %p, addr - stack = %d\n", addr, (char*)addr - stack);
}

void foo() {
    char a[1] = {'1'};
    printf("foo done\n");
}

int func(void *arg) {
    puts("this is func");
    foo();
    swapcontext(&f, &m);
    __builtin_unreachable();
}

void coroutine_test() {
    // 获取当前上下文
    getcontext(&f);

    // 分配栈空间 uc_stack.ss_sp 指向栈顶
    f.uc_stack.ss_sp = stack;
    f.uc_stack.ss_size = STACK_SIZE + offset;
    // f.uc_stack.ss_flags = 0;
    // 指定后继上下文
    // f.uc_link = &m;
    f.uc_link = NULL;

    makecontext(&f, (void (*)(void))func, 0);

    //切换到child上下文，保存当前上下文到main
    swapcontext(&m, &f);
    // 如果设置了后继上下文，func函数指向完后会返回此处 如果设置为NULL，就不会执行这一步
    puts("this is coroutine_test");
    mprotect(stack, page_size, PROT_READ|PROT_WRITE);
    show_satck();
}

int main() {
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_STACK;
    action.sa_sigaction = &sigsegv_handler;
    sigaction(SIGSEGV, &action, NULL);

    stack_t segv_stack;
    segv_stack.ss_sp = valloc(SEGV_STACK_SIZE);
    segv_stack.ss_flags = 0;
    segv_stack.ss_size = SEGV_STACK_SIZE;
    sigaltstack(&segv_stack, NULL);

    size_t alignment = page_size;
    stack = reinterpret_cast<char*>(aligned_alloc(alignment, STACK_SIZE + page_size));
    if (!stack) {
        crash("bad alloc\n");
    }
    memset(stack, 'a', STACK_SIZE + page_size);
    memset(stack, 'p', page_size);;

    show_satck();
    printf("stack = %p\n", stack);
    mprotect(stack, page_size, PROT_NONE);
    // stack[0] = 'k';
    // printf("recover\n");
    // show_satck();

    coroutine_test();
    return 0;
}
