#include <stdio.h>

long long int next = 0;

void bar() {
    asm("pushq (%rbp)");
    asm("movq %rsp, %rbp");
    asm("movq %0, 8(%%rbp)"
        :
        : "r"(next)
        :);
    printf("bar()\n");
}

void foo() {
    printf("foo()\n");
    long long int old_rbp = 0;
    asm("movq (%%rbp), %0"
        : "=r"(old_rbp)
        :
        :);
    printf("old rpb = %lld\n", old_rbp);

    // hack
    asm("movq 8(%%rbp), %0"
        : "=r"(next)
        :
        :);
    asm("movq %0, 8(%%rbp)"
        :
        : "r"(bar)
        :);
}

int main() {
    long long int rbp;
    asm("movq %%rbp, %0"
        : "=r"(rbp)
        :
        :);
    printf("main()\n");
    printf("rbp     = %lld\n", rbp);
    foo();
    printf("exit()\n");
    return 0;
}
