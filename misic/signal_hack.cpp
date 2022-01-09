#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

ucontext_t main_ctx;

void schedule() {
    void* esp;
    printf("enter schedule point, esp = %p\n", &esp);
    setcontext(&main_ctx);
}

void print_ucontext(ucontext_t* ctx) {
    printf("uc_link = %p\n", ctx->uc_link);
    printf("uc_mcontext.gregs[REG_RIP] = %p\n", ctx->uc_mcontext.gregs[REG_RIP]);
    printf("uc_mcontext.gregs[REG_RSP] = %p\n", ctx->uc_mcontext.gregs[REG_RSP]);
}

void hack(ucontext_t* ctx) {
    printf("hack to %p\n", schedule);
    memcpy(&main_ctx, ctx, sizeof(main_ctx));
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
    while (1);
    printf("exit loop\n");

    return 0;
}
