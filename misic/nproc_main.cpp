// clang++ -I../aliclang8/sap_easy/sap/util -I../aliclang8/sap_easy nproc_main.cpp ../aliclang8/sap_easy/sap/util/nproc.cpp

#include <nproc.h>
#include <stdio.h>
#include <iostream>

int main(int ragc, char* argv[]) {
    printf("NPROC_ALL = %lu\n", num_processors(NPROC_ALL));
    printf("NPROC_CURRENT = %lu\n", num_processors(NPROC_CURRENT));
    printf("NPROC_CURRENT_OVERRIDABLE = %lu\n", num_processors(NPROC_CURRENT_OVERRIDABLE));
    return 0;
}
