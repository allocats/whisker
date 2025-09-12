/*
 *
 *  Inspired by tsoding's implementation at https://github.com/tsoding/arena
 *  Original work licensed under MIT License
 *
 *  Usage:
 *
 *      #include "arena.h"
 *      Add arena.c to compilation
 *
 */

#ifndef ARENA_H
#define ARENA_H 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY (4 * 1024) 

#define arena_array(arena, type, count) \
    (type*) arena_alloc(arena, sizeof(type) * (count)) 

#define arena_array_zero(arena, type, count) \
    (type*) arena_memset(arena_alloc(arena, sizeof(type) * (count)), 0, sizeof(type) * (count)) 

typedef struct ArenaBlock {
    struct ArenaBlock* next;
    size_t usage;
    size_t capacity;
    uintptr_t data[];
} ArenaBlock;

typedef struct {
    ArenaBlock* start;
    ArenaBlock* end;
} Arena;

void* arena_alloc(Arena* arena, size_t size);
void* arena_realloc(Arena* arena, void* ptr, size_t old_size, size_t new_size);
void* arena_memset(void* ptr, int value, size_t len);
void* arena_memcpy(void* dest, const void* src, size_t len);
char* arena_strdup(Arena* arena, const char* str);

void arena_reset(Arena* arena);
void arena_free(Arena* arena); 

size_t total_capacity(Arena* arena);
size_t total_usage(Arena* arena); 

#ifdef __cplusplus 
}
#endif

#endif // !ARENA_H
