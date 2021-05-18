/**
 * \file
 * \brief Hash Table
 *
 * This file have...
 */

#ifndef TIGY_HASH_TABLE_H
#define TIGY_HASH_TABLE_H

#include "front-end/syntax/token.h"

/**
 * Allocate table
 *
 * \param None
 * @return Allocate tb
 */
struct hash_table *table_allocate(void);

/**
 *
 * @param table
 * @param key
 * @return
 */
void *table_lookup(struct hash_table* table, const char *key);

/**
 *
 * @param table
 * @param key
 * @param value
 */
void table_insert(struct hash_table* table, const char *key, void *value);

/**
 *
 * @param table
 */
void table_free(struct hash_table **table);

#endif //TIGY_HASH_TABLE_H
