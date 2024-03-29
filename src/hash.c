#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/hash.h"

void hash_init(HashTable* table)
{
    for (size_t i = 0; i < 256; ++i) {
        table->buckets[i] = NULL;
    }
}

void hash_bucket_free(HashBucket* bucket)
{
    if (bucket != NULL) {
        hash_bucket_free(bucket->next);
        free(bucket->value);
        free(bucket);
    }
}

void hash_free(const HashTable* table)
{
    for (size_t i = 0; i < 256; ++i) {
        hash_bucket_free(table->buckets[i]);
    }
}

/*
 * This function implements the FNV-1a hash algorithm, 64 bit size. The
 * implementation applies xor-folding to the result to reduce its size to eight
 * bits. This is more appropriate for the small expected hash sizes in this
 * context.
 */
unsigned char hash_index(const char* key)
{
    unsigned long hash = 14695981039346656037u;
    while (*key) {
        hash ^= (unsigned long)*key++;
        hash *= 1099511628211u;
    }

    // xor-fold
    unsigned char* index = (unsigned char*)&hash;
    for (size_t i = 1; i < sizeof(unsigned long); ++i) {
        index[0] ^= index[i];
    }

    return index[0];
}

void hash_insert(HashTable* table, char* key, void* value)
{
    unsigned char index = hash_index(key);

    HashBucket* bucket = (HashBucket*)malloc(sizeof(HashBucket));
    bucket->key = key;
    bucket->value = value;
    bucket->next = table->buckets[index];

    table->buckets[index] = bucket;
}

const void* hash_retrieve(const HashTable* table, const char* key)
{
    unsigned char index = hash_index(key);

    for (HashBucket* bucket = table->buckets[index]; bucket != NULL; bucket = bucket->next) {
        if (strcmp(key, bucket->key) == 0) {
            return bucket->value;
        }
    }

    return NULL;
}
