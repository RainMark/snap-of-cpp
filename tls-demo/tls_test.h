#pragma once

__attribute__((noinline))
void set_tls_ptr(int*);
__attribute__((noinline))
int* get_tls_ptr();
