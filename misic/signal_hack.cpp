#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


/**
   $ gcc signal_hack.cpp
   $ ./a.out
   $ kill -s SIGUSR1 <pid>
*/

ucontext_t main_ctx;
long long int rip;
long long int rsp;

/**
   schedule() called after user signal handler exit, on kernel back to userspace
   signal related resource already recycled.
*/
void schedule() {
    /**
       the kernel generated ucontext can not used for glibc ucontext.
       getcontext at first and update ip/sp to act like call getcontext on signal interupt point.
    */
    getcontext(&main_ctx);
    main_ctx.uc_mcontext.gregs[REG_RIP] = rip;
    main_ctx.uc_mcontext.gregs[REG_RSP] = rsp;
    printf("enter schedule point\n");
    setcontext(&main_ctx);
}

void print_ucontext(ucontext_t* ctx) {
    printf("uc_link = %p\n", ctx->uc_link);
    printf("uc_mcontext.gregs[REG_RIP] = %p\n", ctx->uc_mcontext.gregs[REG_RIP]);
    printf("uc_mcontext.gregs[REG_RSP] = %p\n", ctx->uc_mcontext.gregs[REG_RSP]);
}

void hack(ucontext_t* ctx) {
    printf("hack to %p\n", schedule);
    rip = ctx->uc_mcontext.gregs[REG_RIP];
    rsp = ctx->uc_mcontext.gregs[REG_RSP];
    ctx->uc_mcontext.gregs[REG_RIP] = (long long int) schedule;
}

void sighandler(int signum, siginfo_t *info, void *data) {
    printf("enter signal handler %d on %p\n", signum, info->si_addr);
    ucontext_t* ctx = (ucontext_t*) data;
    print_ucontext(ctx);
    hack(ctx);
    printf("exit signal handler\n");
}

int main() {
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_STACK;
    action.sa_sigaction = &sighandler;
    sigaction(SIGUSR1, &action, NULL);

    printf("enter loop\n");
    while (1) {
        printf("enter sleep 1\n");
        sleep(1);
        printf("exit sleep 1\n");
    }
    printf("exit loop\n");

    return 0;
}
