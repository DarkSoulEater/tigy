//
// Created by eleno on 27.02.2021.
//

#ifndef TIGY_HASH_TABLE_H
#define TIGY_HASH_TABLE_H

#include "../token.h"

struct hash_table {
    int buffer_size;
    int size;
    struct binding {
    	char key[IDENTIFIER_LENGTH];
    	void *value;
    } **data;
};

struct hash_table *table_allocate(void);
void *table_lookup(struct hash_table* table, const char *key);
void table_insert(struct hash_table* table, const char *key, void *value);
void table_free(struct hash_table **table);

#endif //TIGY_HASH_TABLE_H
