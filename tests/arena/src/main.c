#include "arena.h"

#include <stdio.h>

#define SIZE 512

static ArenaAllocator arena = {0};

int main(void) {
    init_arena(&arena, 512);

    char* s = arena_alloc(&arena, SIZE);
    arena_memset(s, 'S', SIZE - 1);
    s[SIZE - 1] = 0;

    printf("%s\n", s);
}
