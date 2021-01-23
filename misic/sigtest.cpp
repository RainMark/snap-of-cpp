// gcc ucontext-example.c -o ucontext-example --std=gnu99
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define STACK_SIZE 4096
#define MAX_COROS 8

int current = 0;  // 当前协程

typedef struct coro_t {
    int used;
    ucontext_t context;
    char* stack;
    char* more_satck;
    void* (*func)(void *arg);
    void *arg;
    void *exit_status;
} coro_t;

static coro_t coro_slots[MAX_COROS];  // 用以维护当前线程/进程的所有协程

// 从slots里调度一个协程执行
void coro_schedule(void) {
    int i;
    for (i = (current + 1) % MAX_COROS; i != current; i = (i + 1) % MAX_COROS)
        if (coro_slots[i].used) break;  // 找到slots里current后面的第一个active的协程

    int prev = current;
    current = i;
    printf("coro_schedule stop coro-%d, and running coro-%d\n", prev, current);
    swapcontext(&coro_slots[prev].context, &coro_slots[current].context);  // 保存旧状态，切换到新状态
}

void coro_exit(void) {
    void *exit_status = coro_slots[current].func(coro_slots[current].arg);
    coro_slots[current].exit_status = exit_status;
    coro_slots[current].used = 0;
    coro_schedule();
}

int coro_create(void* (*start_routine)(void*), void *arg) {
    static int last_used = 0;
    int i, tid;

    for (i = (last_used + 1) % MAX_COROS; i != last_used; i = (i + 1) % MAX_COROS)
        if (!coro_slots[i].used) break;
    if (i == last_used)
        return -1;
    last_used = i;
    tid = i;

    /*
      typedef struct ucontext {
      struct ucontext *uc_link;       // 如果当前context终结，将恢复执行uc_link指向的context
      sigset_t         uc_sigmask;    // 当前context阻止的信号集合，sigprocmask(2)
      stack_t          uc_stack;      // 当前context使用的栈，sigaltstack(2)
      mcontext_t       uc_mcontext;   // machine-specific，包含调用者线程的寄存器等
      ...
      } ucontext_t;
    */
    // 原型：int getcontext(ucontext_t *ucp);
    getcontext(&coro_slots[tid].context);  // 将当前的、active的context保存到ucp中
    coro_slots[tid].context.uc_stack.ss_sp = coro_slots[tid].stack;  // 调用makecontext之前必须设定uc_stack，和uc_link
    coro_slots[tid].context.uc_stack.ss_size = STACK_SIZE;
    coro_slots[tid].context.uc_link = &coro_slots[0].context;

    coro_slots[i].used = 1;
    coro_slots[i].func = start_routine;
    coro_slots[i].arg = arg;
    coro_slots[i].exit_status = 0;
    // 原型：makecontext(ucontext_t* ucp, void (*func)(), int argc, ...);
    makecontext(&coro_slots[i].context, coro_exit, 0);  // 修改getcontext获取的ucp，如果ucp后面通过setcontext/swapcontext被激活，则函数func将被调用
    return 0;
}

void* coro_func(void *arg) {
    int dummy[2016]={0};
    int dummy2[10]={0};
    // printf("running coro-%d coro_func(%s)\n", current, (char*)arg);
    printf("dummy = %p\n", dummy);
    printf("dummy2 = %p\n", dummy2);
    coro_schedule();
    __builtin_unreachable();
}

void coro_init() {
    size_t page_size = getpagesize();
    size_t alignment = page_size;
    printf("page size = %zu\n", page_size);
    for (auto i = 0; i < MAX_COROS; ++i) {
        void* mem = aligned_alloc(alignment, STACK_SIZE + page_size);
        if (!mem) {
            printf("bad alloc\n");
            exit(0);
        }
        coro_slots[i].stack = (char *)mem;
        mprotect(mem, page_size, PROT_NONE);
        printf("err = %s\n", strerror(errno));
        printf("coro_slots[i].stack = %p\n", coro_slots[i].stack);
    }
}

#define crash(fmt, args...)                     \
    do {                                        \
        printf(fmt, ##args);                    \
        exit(-1);                               \
    } while (0)

void sigsegv_handler(int signum, siginfo_t *info, void *data) {
    void *addr = info->si_addr;
    crash("Segmentation fault from reading the address %p.", addr);
}

int main(int argc, char *argv[]) {
    coro_init();

    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_flags = SA_SIGINFO|SA_STACK;
    action.sa_sigaction = &sigsegv_handler;
    sigaction(SIGSEGV, &action, NULL);

    coro_create(coro_func, (void*)"coro1 args");  // 创建子协程，但仍未触发调用
    coro_create(coro_func, (void*)"coro2 args");

    coro_schedule();

    return 0;
}

/*
 * coro_schedule stop coro-0, and running coro-1
 * running coro-1 coro_func(coro1 args)
 * coro_schedule stop coro-1, and running coro-2
 * running coro-2 coro_func(coro2 args)
 * coro_schedule stop coro-2, and running coro-2
 * coro_schedule stop coro-2, and running coro-2
 */
