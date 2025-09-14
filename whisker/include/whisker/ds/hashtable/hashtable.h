#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../../allocators/arena.h"

#include <stddef.h>
#include <stdint.h>

#define HT_DEFAULT_CAPACITY 16

typedef struct HashItem {
    void* key;
    void* value;
    uint32_t hash;
    struct HashItem* next;
} HashItem;

typedef struct {
    HashItem** items;
    size_t key_size;
    size_t value_size;
    size_t count;
    size_t capacity;
    ArenaAllocator allocator;
} HashTable;

HashTable* create_hashtable(size_t key_size, size_t value_size, size_t capacity);
void destroy_hashtable(HashTable* ht);

void ht_insert(HashTable* ht, void* key, void* value);
void print_ht(HashTable* ht);

#endif // !HASHTABLE_H
