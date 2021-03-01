//
// Created by eleno on 27.02.2021.
//

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 8
#define REBUILD_FACTOR 0.75f

static int hash_function(char* str, int table_size, int key);
static int get_hash_ind (char *str, int table_size);
static int get_hash_step(char *str, int table_size);
static void resize      (struct hashtable* tb);

void table_create(struct hashtable* tb)
{
        tb->buffer_size = DEFAULT_SIZE;
        tb->size = 0;
        tb->arr = malloc(sizeof(struct node*) * DEFAULT_SIZE);
        int i;
        for (i = 0; i < DEFAULT_SIZE; ++i) {
                tb->arr[i] = NULL;
        }
}

int table_find(struct hashtable* tb, const char *key)
{
        int ind = get_hash_ind(key, tb->buffer_size);
        int step = get_hash_step(key, tb->buffer_size);
        int i = 0;
        while (tb->arr[ind] != NULL && i < tb->buffer_size) {
                if (strcmp(key, tb->arr[ind]->value) == 0)
                        return 1;
                ind = (ind + step) % tb->buffer_size;
                ++i;
        }
        return 0;
}

int table_insert(struct hashtable* tb, const char *key)
{
        if (tb->size + 1 > (int)(REBUILD_FACTOR * tb->buffer_size)) {
                resize(tb);
        }

        int ind = get_hash_ind(key, tb->buffer_size);
        int step = get_hash_step(key, tb->buffer_size);
        int i = 0;
        while (tb->arr[ind] != NULL && i < tb->buffer_size) {
                if (strcmp(tb->arr[ind]->value, key) == 0)
                        return 0;
                ind = (ind + step) % tb->buffer_size;
                ++i;
        }

        tb->arr[ind] = malloc(sizeof(struct node));
        strncpy(tb->arr[ind]->value, key, VALLEN);
        ++tb->size;
}

int hash_function(char* str, int table_size, int key)
{
        int hash_res = 0;
        while (*str != '\0')
                hash_res = (key * hash_res + *str++) % table_size;
        return (hash_res * 2 + 1) % table_size;
}

int get_hash_ind(char *str, int table_size)
{
        return hash_function(str, table_size, table_size - 1);
}

int get_hash_step(char *str, int table_size)
{
        return hash_function(str, table_size, table_size + 1);
}

void resize(struct hashtable* tb)
{
        int old_size = tb->buffer_size;
        tb->buffer_size *= 2;
        tb->size = 0;
        struct node **old_arr = tb->arr;
        tb->arr = malloc(sizeof (struct node*) * tb->buffer_size);
        int i;
        for (i = 0; i < tb->buffer_size; ++i) {
                tb->arr[i] = NULL;
        }
        for (i = 0; i < old_size; ++i) {
                if (old_arr[i] != NULL) {
                        table_insert(tb, old_arr[i]->value);
                        free(old_arr[i]);
                }
        }
        free(old_arr);
}
