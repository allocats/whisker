#include "pool.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void pool_free_all(PoolAllocator* pool) {
    if (!pool) {
        return;
    }

    size_t count = pool -> elem_count;
    
    for (size_t i = 0; i < count; i++) {
        void* ptr = &pool -> memory[i * pool -> elem_size];
        FreeNode* node = (FreeNode*) ptr;

        node -> next = pool -> free_head;
        pool -> free_head = node;
    }
}

void pool_init(PoolAllocator* pool, size_t elem_size, size_t n) {
    if (!pool) {
        return;
    }

    elem_size = elem_size >= sizeof(FreeNode*) ? elem_size : sizeof(FreeNode*);

    pool -> elem_size = elem_size;
    pool -> elem_count = n;
    pool -> capacity = elem_size * n;
    pool -> free_head = NULL;
    pool -> memory = malloc(pool -> capacity);
    assert(pool -> memory);

    pool_free_all(pool);
}

void* pool_alloc(PoolAllocator* pool) {
    if (!pool) {
        return NULL;
    }

    FreeNode* node = pool -> free_head;
    assert(node);

    pool -> free_head = pool -> free_head -> next;
    return memset(node, 0, pool -> elem_size);
}

void pool_free(PoolAllocator* pool, void* ptr) {
    if (!ptr || !pool) {
        return;
    }

    void* start = pool -> memory;
    if (ptr < start || ptr >= start + pool -> capacity) {
        return;
    }

    FreeNode* node = (FreeNode*) ptr;
    node -> next = pool -> free_head;
    pool -> free_head = node;
}

void pool_destroy(PoolAllocator* pool) {
    if (pool && pool -> memory) {
        free(pool -> memory);
        pool -> memory = NULL;
    }
}
