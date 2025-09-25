#include "arena.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>

#define UNLIKELY(x) __builtin_expect(x, 0)
#define LIKELY(x) __builtin_expect(x, 1)

extern void* arena_realloc_avx2(ArenaAllocator* arena, void* ptr, size_t old_size, size_t new_size);
extern void* arena_memcpy_avx2(void* dest, const void* src, size_t len);
extern void* arena_memset_avx2(void* ptr, int value, size_t len);

static void* (*arena_realloc_impl)(ArenaAllocator* arena, void* ptr, size_t old_size, size_t new_size);
static void* (*arena_memcpy_impl)(void* dest, const void* src, size_t len);
static void* (*arena_memset_impl)(void* ptr, int value, size_t len);

__attribute__((constructor)) static void arena_dispatch(void) {
    __builtin_cpu_init();

    if (__builtin_cpu_supports("avx2")) {
        arena_realloc_impl = arena_realloc_avx2;
        arena_memcpy_impl = arena_memcpy_avx2;
        arena_memset_impl = arena_memset_avx2;
    } 
}

void* arena_realloc(ArenaAllocator* arena, void* ptr, size_t old_size, size_t new_size) {
    return arena_realloc_impl(arena, ptr, old_size, new_size);
}

void* arena_memcpy(void* dest, const void* src, size_t len) {
    return arena_memcpy_impl(dest, src, len);
}

void* arena_memset(void* ptr, int value, size_t len) {
    return arena_memset_impl(ptr, value, len);
}

inline size_t align_size(size_t size) {
    return (size + 31) & ~(31);
}

void init_arena(ArenaAllocator* arena, size_t default_capacity) {
    assert(arena);
    arena -> start = NULL;
    arena -> end = NULL;
    arena -> default_capacity = default_capacity == 0 ? ARENA_DEFAULT_CAPACITY : align_size(default_capacity);
}

static ArenaBlock* new_block(size_t default_capacity, size_t size) {
    size_t capacity = default_capacity;

    while (UNLIKELY(size > capacity * sizeof(uintptr_t))) {
        capacity *= 2;
    }

    size_t bytes = capacity * sizeof(uintptr_t);
    size_t total_size = sizeof(ArenaBlock) + bytes;
    size_t aligned_size = align_size(total_size);
    ArenaBlock* block = (ArenaBlock*) aligned_alloc(32, aligned_size);
    assert(block);

    block -> next = NULL;
    block -> usage =  0;
    block -> capacity = bytes;

    return block;
}

static inline void free_block(ArenaBlock* block) {
    free(block);
}

void* arena_alloc(ArenaAllocator* arena, size_t size) {
    ArenaBlock* current = arena -> end;

    if (UNLIKELY(!current)) {
        current = new_block(arena -> default_capacity, size);
        arena -> start = arena -> end = current;

        void* result = (char*) current -> data + current -> usage;
        current -> usage += size;
        return result;
    }

    while (UNLIKELY(arena -> end -> usage + size > arena -> end -> capacity && arena -> end -> next != NULL)) {
        arena -> end = arena -> end -> next;
    } 

    if (UNLIKELY(arena -> end -> usage + size > arena -> end -> capacity)) {
            ArenaBlock* block = new_block(arena -> default_capacity, size);
            arena -> end -> next = block;
            arena -> end = block;
    }

    void* result = (char*) arena -> end -> data + arena -> end -> usage;
    arena -> end -> usage += size;
    return result;
}



char* arena_strdup(ArenaAllocator* arena, const char* str) {
    size_t len = strlen(str);
    char* duplicate = (char*) arena_alloc(arena, len + 1);

    arena_memcpy(duplicate, str, len + 1);
    duplicate[len] = '\0';

    return duplicate;
}

inline void arena_reset(ArenaAllocator* arena) {
    for (ArenaBlock* block = arena -> start; block != NULL; block = block -> next) {
        block -> usage = 0;
    }
    arena -> end = arena -> start;
}

void arena_free(ArenaAllocator* arena) {
    ArenaBlock* block = arena -> start;

    while (block != NULL) {
        ArenaBlock* previous = block;
        block = block -> next;
        free_block(previous);
    }

    arena -> start = NULL;
    arena -> end = NULL;
}

size_t total_capacity(ArenaAllocator* arena) {
    ArenaBlock* current = arena -> start;
    size_t total = 0;

    while (current != NULL) {
        total += current -> capacity;
        current = current -> next;
    }
    
    return total;
}

size_t total_usage(ArenaAllocator* arena) {
    ArenaBlock* current = arena -> start;
    size_t total = 0;

    while (current != NULL) {
        total += current -> usage;
        current = current -> next;
    }
    
    return total;
}
