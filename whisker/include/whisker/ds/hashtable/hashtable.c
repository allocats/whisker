#include "hashtable.h"

#include "../../macros.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t hash_key(void* ptr, size_t n) {
    char* p = (char*) ptr;
    uint32_t hash = 2166136261u;
    
    for (size_t i = 0; i < n; i++) {
        hash ^= *p++;
        hash *= 16777619u;
    }

    return hash;
}

static inline size_t align_capacity(size_t n) {
    return 1 << (32 - __builtin_clz(n - 1));
}

HashTable* create_hashtable(size_t key_size, size_t value_size, size_t capacity) {
    assert(key_size > 0);
    assert(value_size > 0);

    capacity = capacity == 0 ? HT_DEFAULT_CAPACITY : align_capacity(capacity);
    HashTable* ht = malloc(sizeof(*ht));
    if (UNLIKELY(!ht)) {
        return NULL;
    }

    size_t size = ((sizeof(HashItem) + key_size + value_size - (sizeof(void*) * 2)) * capacity) / sizeof(uintptr_t);
    init_arena(&ht -> allocator, size);

    ht -> items = arena_alloc(&ht -> allocator, capacity * sizeof(HashItem*));
    if (UNLIKELY(!ht -> items)) {
        arena_free(&ht -> allocator);
        free(ht);
        return NULL;
    }

    ht -> key_size = key_size;
    ht -> value_size = value_size;
    ht -> count = 0;
    ht -> capacity = capacity;

    return ht;
}

void destroy_hashtable(HashTable* ht) {
    if (LIKELY(ht != NULL)) {
        arena_free(&ht -> allocator);
        free(ht);
        ht = NULL;
    }
}

void ht_insert(HashTable* ht, void* key, void* value) {
    assert(ht);
    assert(key);
    assert(value);

    size_t key_size = ht -> key_size;
    size_t value_size = ht -> value_size;

    uint32_t hash = hash_key(key, key_size);
    size_t idx = hash & (ht -> capacity - 1); 

    HashItem* item = ht -> items[idx];

    W_TODO("collision");

    item -> hash = hash;
    memcpy(item -> key, key, key_size);
    memcpy(item -> value, value, value_size);

    item -> next = ht -> items[idx];
    ht -> items[idx] = item;
    ht -> count++;
}

void print_ht(HashTable* ht) {
    assert(ht);

    printf("\n *** HashTable ***\n\n");
    printf("Address: %p\n", ht);
    printf("Count: %zu\n", ht -> count);
    printf("Capacity: %zu\n", ht -> capacity);
    printf("Key size: %zu\n", ht -> key_size);
    printf("Value size: %zu\n\n", ht -> value_size);

    for (size_t i = 0; i < ht -> capacity; i++) {
        HashItem* item = ht -> items[i];

        printf("Node %zu:\n", i);
        printf("  Address: %p\n", item);
        printf("  Key: %p\n", item -> key);
        printf("  Value: %p\n\n", item -> value);
    }
}
