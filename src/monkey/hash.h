#ifndef MONKEY_HASH_H_
#define MONKEY_HASH_H_

typedef struct HashBucket HashBucket;
struct HashBucket {
    char* key;
    void* value;
    HashBucket* next;
};

typedef struct HashTable HashTable;
struct HashTable {
    HashBucket* buckets[256];
};

void hash_init(HashTable*);
void hash_free(const HashTable*);
void hash_insert(HashTable*, char*, void*);
const void* hash_retrieve(const HashTable*, const char*);

#endif // MONKEY_HASH_H_
