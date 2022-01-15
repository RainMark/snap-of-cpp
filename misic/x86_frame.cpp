#include <stdio.h>

long long int next = 0;

// asm("ojbk:\n"
//     "   movl $1, %eax\n"
//     "   retq"
//     );

// extern int ojbk() asm ("ojbk");
extern int xxx() asm("xxx");

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

int f() {
    asm("movl $10086, %eax");
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
    // printf("ojbk(): %d\n", ojbk());
    printf("f() %d\n", f());
    printf("xxx() %d\n", xxx());
    printf("exit()\n");
    return 0;
}
