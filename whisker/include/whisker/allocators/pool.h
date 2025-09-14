#ifndef POOL_H
#define POOL_H

#include <stddef.h>

typedef struct FreeNode {
    struct FreeNode* next;
} FreeNode;

typedef struct {
    unsigned char* memory;
    size_t elem_size;
    size_t elem_count;
    size_t capacity;
    FreeNode* free_head;
} PoolAllocator;

void pool_init(PoolAllocator* pool, size_t elem_size, size_t n);
void pool_destroy(PoolAllocator* pool);

void* pool_alloc(PoolAllocator* pool);
void pool_free(PoolAllocator* pool, void* ptr);
void pool_reset(PoolAllocator* pool);

#endif // !POOL_H
