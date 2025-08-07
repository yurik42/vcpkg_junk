/*
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t total = 0;
    size_t curr = 1;

    while (1) {
        char *buff = malloc(curr);
        if (!buff)
            break;
        total += curr;
        curr *= 2;
    }

    printf("total == 0x%016llx\ncurr  == 0x%016llx\n", total, curr);
    return 0;
}
