#include <iostream>
#include <exception>
#include <ucontext.h>

#define STACK_SIZE 1024

///////////////////////////
// forward declaration for splitstack-functions defined in libgcc
// https://github.com/gcc-mirror/gcc/blob/master/libgcc/generic-morestack.c
// https://github.com/gcc-mirror/gcc/blob/master/libgcc/config/i386/morestack.S

struct dynamic_allocation_blocks {
  /* The next block in the list.  */
  struct dynamic_allocation_blocks *next;
  /* The size of the allocated memory.  */
  size_t size;
  /* The allocated memory.  */
  void *block;
};

struct stack_segment {
  /* The previous stack segment--when a function running on this stack
     segment returns, it will run on the previous one.  */
  struct stack_segment *prev;
  /* The next stack segment, if it has been allocated--when a function
     is running on this stack segment, the next one is not being
     used.  */
  struct stack_segment *next;
  /* The total size of this stack segment.  */
  size_t size;
  /* The stack address when this stack was created.  This is used when
     popping the stack.  */
  void *old_stack;
  /* A list of memory blocks allocated by dynamic stack
     allocation.  */
  struct dynamic_allocation_blocks *dynamic_allocation;
  /* A list of dynamic memory blocks no longer needed.  */
  struct dynamic_allocation_blocks *free_dynamic_allocation;
  /* An extra pointer in case we need some more information some
     day.  */
  void *extra;
};

enum splitstack_context_offsets {
  MORESTACK_SEGMENTS = 0,
  CURRENT_SEGMENT = 1,
  CURRENT_STACK = 2,
  STACK_GUARD = 3,
  INITIAL_SP = 4,
  INITIAL_SP_LEN = 5,
  BLOCK_SIGNALS = 6,

  NUMBER_OFFSETS = 10
};

extern "C" {
  void *__splitstack_makecontext(std::size_t, void* [NUMBER_OFFSETS], std::size_t*);

  void __splitstack_setcontext(void* [NUMBER_OFFSETS]);

  void __splitstack_getcontext(void* [NUMBER_OFFSETS]);

  void __splitstack_releasecontext(void* [NUMBER_OFFSETS]);

  void __splitstack_resetcontext(void* [NUMBER_OFFSETS]);

  void __splitstack_block_signals_context(void* [NUMBER_OFFSETS], int* new_value, int* old_value);
}

///////////////////////////

struct stack_context {
  using segments_context = void* [NUMBER_OFFSETS];

  std::size_t size = 0;
  void* sp = nullptr;
  segments_context segments_ctx{};
  segments_context link{};
};

stack_context sctx;
ucontext_t m, f;

void print_dynamic_blocks (struct dynamic_allocation_blocks *p) {
  while (p) {
    std::cout << "block: " << p << std::endl;
    std::cout << "block size: " << p->size << std::endl;
    p = p->next;
  }
}

void print_stack_usage(stack_context& sc) {
  auto ss = reinterpret_cast<stack_segment*>(sc.segments_ctx[MORESTACK_SEGMENTS]);
  while (ss) {
    std::cout << "segment: " << ss << std::endl;
    std::cout << "segment size: " << ss->size << std::endl;
    print_dynamic_blocks(ss->dynamic_allocation);
    print_dynamic_blocks(ss->free_dynamic_allocation);
    ss = ss->next;
  }
}

void make_stack(stack_context& sc, std::size_t stack_size) {
  sc.size = stack_size;
  void* vp = __splitstack_makecontext(stack_size, sc.segments_ctx, &sc.size);
  if (!vp) {
    throw std::bad_alloc();
  }
  __splitstack_getcontext(sc.link);
  __splitstack_setcontext(sc.segments_ctx);
  // sctx.size is already filled by __splitstack_makecontext
  sc.sp = static_cast<char*>(vp);
  int off = 0;
  __splitstack_block_signals_context(sc.segments_ctx, &off, 0);
}

void bar() {
  char b[STACK_SIZE * 1024 * 100] = {0};
}

void foo() {
  char b[STACK_SIZE * 1024 * 100] = {0};
  bar();
  std::cin.ignore();
  std::cout << "foo()" << std::endl;
}

int func(void *arg) {
  std::cout << "func()" << std::endl;
  foo();
  swapcontext(&f, &m);
  __builtin_unreachable();
}

void coroutine_test() {
  getcontext(&f);
  make_stack(sctx, STACK_SIZE);

  f.uc_stack.ss_sp = sctx.sp;
  f.uc_stack.ss_size = sctx.size;
  // f.uc_stack.ss_flags = 0;

  // We call swapcontext() at func end, no need to set uc_link
  // f.uc_link = &m;
  f.uc_link = nullptr;

  makecontext(&f, (void (*)(void))func, 0);
  swapcontext(&m, &f);
  std::cout << "coroutine_test()" << std::endl;

  print_stack_usage(sctx);
  __splitstack_setcontext(sctx.link);
  __splitstack_releasecontext(sctx.segments_ctx);
}

int main() {
  int a[STACK_SIZE * 8] = {0};
  coroutine_test();

  std::cout << "main thread stack info" << std::endl;
  stack_context sc;
  __splitstack_getcontext(sc.segments_ctx);
  print_stack_usage(sc);
  std::cin.ignore();
  return 0;
}
