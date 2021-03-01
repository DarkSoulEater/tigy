//b 
// Created by eleno on 27.02.2021.
//

#ifndef TIGY_HASHTABLE_H
#define TIGY_HASHTABLE_H

#define VALLEN 20

struct hashtable {
    struct node {
        int state;
        char value[VALLEN];
    };
    int buffer_size;
    int size;
    int table_size;
    struct node **arr;
};

void table_create(struct hashtable* tb);

int table_find(struct hashtable* tb, const char *key);

int table_insert(struct hashtable* tb, const char *key);

int table_remove(struct hashtable* tb, const char *key);


#endif //TIGY_HASHTABLE_H
