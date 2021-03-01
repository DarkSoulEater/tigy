//
// Created by eleno on 27.02.2021.
//

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define DEFAULT_SIZE 8
#define REBUILD_FACTOR 0.75f

static int hash_function(char* str, int table_size, int key);
static int get_hash_ind (char *str, int table_size);
static int get_hash_step(char *str, int table_size);
static void resize      (struct hashtable* tb, int k);

void table_create(struct hashtable* tb)
{
        tb->buffer_size = DEFAULT_SIZE;
        tb->size = tb->table_size = 0;
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
                if (tb->arr[ind]->state && !strcmp(key, tb->arr[ind]->value))
                        return 1;
                ind = (ind + step) % tb->buffer_size;
                ++i;
        }
        return 0;
}

int table_insert(struct hashtable* tb, const char *key)
{
        if (tb->size + 1 > (int)(REBUILD_FACTOR * tb->buffer_size)) {
                resize(tb, 2);
        } else if (tb->table_size > 2 * tb->size) {
                resize(tb, 1);
        }
        int ind = get_hash_ind(key, tb->buffer_size);
        int step = get_hash_step(key, tb->buffer_size);
        int i = 0, first_del = -1;
        while (tb->arr[ind] != NULL && i < tb->buffer_size) {
                if (tb->arr[ind]->state && !strcmp(tb->arr[ind]->value, key)) {
                        return 0;
                }
                if (first_del == -1 && !tb->arr[ind]->state) {
                        first_del = ind;
                }
                ind = (ind + step) % tb->buffer_size;
                ++i;
        }
        if (first_del != -1) {
                tb->arr[first_del]->state = 1;
                strncpy(tb->arr[first_del]->value, key, VALLEN);
        } else {
                tb->arr[ind] = malloc(sizeof(struct node));
                tb->arr[ind]->state = 1;
                strncpy(tb->arr[ind]->value, key, VALLEN);
                tb->table_size += 1;
        }
        ++tb->size;
}

int table_remove(struct hashtable* tb, const char *key)
{
        int ind = get_hash_ind(key, tb->buffer_size);
        int step = get_hash_step(key, tb->buffer_size);
        int i = 0;
        while (tb->arr[ind] != NULL && i < tb->buffer_size) {
                if (tb->arr[ind]->state && !strcmp(key, tb->arr[ind]->value)) {
                        tb->arr[ind]->state = 0;
                        --(tb->size);
                        return 1;
                }
                ind = (ind + step) % tb->buffer_size;
                ++i;
        }
        return 0;
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

void resize(struct hashtable* tb, int k)
{
        int old_size = tb->buffer_size;
        tb->buffer_size *= k;
        tb->size = tb->table_size = 0;
        struct node **old_arr = tb->arr;
        tb->arr = malloc(sizeof (struct node*) * tb->buffer_size);
        int i;
        for (i = 0; i < tb->buffer_size; ++i) {
                tb->arr[i] = NULL;
        }
        for (i = 0; i < old_size; ++i) {
                if (old_arr[i] != NULL) {
                        if (old_arr[i]->state) table_insert(tb, old_arr[i]->value);
                        free(old_arr[i]);
                }
        }
        free(old_arr);
}
