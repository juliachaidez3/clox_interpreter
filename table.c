#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

static uint32_t hashBits(uint64_t hash) {
    hash = ~hash + (hash << 18);
    hash ^= hash >> 31;
    hash *= 21;
    hash ^= hash >> 11;
    hash += hash << 6;
    hash ^= hash >> 22;
    return (uint32_t)hash;
}

static uint32_t hashValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            return AS_BOOL(value) ? 3 : 1;

        case VAL_NIL:
            return 2;

        case VAL_NUMBER: {
            union {
                double num;
                uint64_t bits;
            } data;
            data.num = AS_NUMBER(value);
            return hashBits(data.bits);
        }

        case VAL_OBJ:
            if (IS_STRING(value)) return AS_STRING(value)->hash;
            return hashBits((uint64_t)(uintptr_t)AS_OBJ(value));
    }

    return 0;
}

static bool isEmptyEntry(Entry* entry) {
    return IS_NIL(entry->key);
}

static bool isTombstone(Entry* entry) {
    return IS_NIL(entry->key) && IS_BOOL(entry->value) && AS_BOOL(entry->value);
}

static Entry* findEntry(Entry* entries, int capacity, Value key) {
    uint32_t index = hashValue(key) % capacity;
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

        if (isEmptyEntry(entry)) {
            if (IS_NIL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL) tombstone = entry;
            }
        } else if (valuesEqual(entry->key, key)) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

static void adjustCapacity(Table* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NIL_VAL;
        entries[i].value = NIL_VAL;
    }

    table->count = 0;

    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (isEmptyEntry(entry)) continue;

        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

bool tableGet(Table* table, Value key, Value* value) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (isEmptyEntry(entry)) return false;

    *value = entry->value;
    return true;
}

bool tableSet(Table* table, Value key, Value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry* entry = findEntry(table->entries, table->capacity, key);
    bool isNewKey = isEmptyEntry(entry);

    if (isNewKey && IS_NIL(entry->value)) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool tableDelete(Table* table, Value key) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (isEmptyEntry(entry)) return false;

    entry->key = NIL_VAL;
    entry->value = BOOL_VAL(true);
    return true;
}

void tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (!isEmptyEntry(entry)) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

ObjString* tableFindString(Table* table, const char* chars, int length,
                           uint32_t hash) {
    if (table->count == 0) return NULL;

    uint32_t index = hash % table->capacity;
    for (;;) {
        Entry* entry = &table->entries[index];

        if (isEmptyEntry(entry)) {
            if (IS_NIL(entry->value)) return NULL;
        } else if (IS_STRING(entry->key)) {
            ObjString* key = AS_STRING(entry->key);
            if (key->length == length &&
                key->hash == hash &&
                memcmp(key->chars, chars, length) == 0) {
                return key;
            }
        }

        index = (index + 1) % table->capacity;
    }
}