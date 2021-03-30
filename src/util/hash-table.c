//
// Created by eleno on 30.03.2021.
//


#include "hash-table.h"
#include "assert.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 8
#define REBUILD_FACTOR 0.75f

static int hash_function(const char *str, int table_size, int key) {
        int hash_res = 0;
        while (*str != '\0')
                hash_res = (key * hash_res + *str++) % table_size;
        return (hash_res * 2 + 1) % table_size;
}

static int get_hash_index(const char *str, int table_size) {
        return hash_function(str, table_size, table_size - 1);
}

static int get_hash_step(const char *str, int table_size) {
        return hash_function(str, table_size, table_size + 1);
}

struct hash_table {
    int buffer_size;
    int size;
    struct binding {
        char key[IDENTIFIER_LENGTH];
        void *value;
    } **data;
};


static void resize(struct hash_table* table) {
        table->buffer_size *= 2;
        table->size = 0;
        struct binding **old_data = table->data;
        table->data = malloc(sizeof(struct binding *) * table->buffer_size);
        for (int i = 0; i < table->buffer_size; ++i) {
                table->data[i] = NULL;
        }
        for (int i = 0; i < table->buffer_size / 2; ++i) {
                table_insert(table, old_data[i]->key, old_data[i]->value);
                free(old_data[i]);
        }
        free(old_data);
}

struct hash_table *table_allocate(void) {
        struct hash_table* table = malloc(sizeof(struct hash_table));
        table->buffer_size = DEFAULT_SIZE;
        table->size = 0;
        table->data = malloc(sizeof(struct binding *) * table->buffer_size);
        for (int i = 0; i < table->buffer_size; ++i) {
                table->data[i] = NULL;
        }
        return table;
}

void *table_lookup(struct hash_table* table, const char *key) {
        assert(key != NULL);
        int index = get_hash_index(key, table->buffer_size);
        int step = get_hash_step(key, table->buffer_size);
        int step_count = 0;
        while (table->data[index] != NULL && step_count < table->buffer_size) {
                if (strcmp(key, table->data[index]->key) == 0)
                        return table->data[index]->value;
                index = (index + step) % table->buffer_size;
                step_count++;
        }
        return NULL;
}

void table_insert(struct hash_table* table, const char *key, void *value) {
        assert(key != NULL && value != NULL);
        if (table->size == table->buffer_size)
                resize(table);
        int index = get_hash_index(key, table->buffer_size);
        int step = get_hash_step(key, table->buffer_size);
        while (table->data[index] != NULL) {
                assert(strcmp(table->data[index]->key, key));
                index = (index + step) % table->buffer_size;
        }
        table->data[index] = malloc(sizeof(struct binding));
        strcpy(table->data[index]->key, key);
        table->data[index]->value = value;
        table->size++;
}

void table_free(struct hash_table **table) {
        for (int i = 0; i < (*table)->buffer_size; ++i) {
                if ((*table)->data != NULL)
                        free((*table)->data[i]);
        }
        free((*table)->data);
        free(*table);
        table = NULL;
}
