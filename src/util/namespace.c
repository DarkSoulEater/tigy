//
// Created by Artyom Plevako on 30.03.2021.
//

#include <assert.h>
#include <stdlib.h>

#include "hash-table.h"
#include "namespace.h"

struct namespace {
	struct scope {
		struct hash_table *hash_table;
		struct scope *link;
	} *data;
};

struct namespace *namespace_allocate(void)
{
	struct namespace *space = malloc(sizeof(struct namespace));
	space->data = NULL;
	return space;
}

void namespace_begin_scope(struct namespace *space)
{
	assert(space != NULL);
	struct scope *scope = malloc(sizeof(struct scope));
	scope->hash_table = table_allocate();
	scope->link = space->data;
	space->data = scope;
}

void namespace_end_scope(struct namespace *space)
{
	assert(space != NULL);
	space->data = space->data->link;
}

void namespace_insert(struct namespace *space, const char *key, void *value)
{
	assert(space != NULL && key != NULL && value != NULL);
	table_insert(space->data->hash_table, key, value);
}

void *namespace_lookup(struct namespace *space, const char *key)
{
	assert(space != NULL && key != NULL);
	return table_lookup(space->data->hash_table, key);
}

void *namespace_deep_lookup(struct namespace *space, const char *key)
{
	assert(space != NULL && key != NULL);
	for (struct scope *data = space->data; data != NULL; data = data->link)
		if (table_lookup(data->hash_table, key) != NULL)
			return table_lookup(data->hash_table, key);
	return NULL;
}

void namespace_free(struct namespace **space)
{
	assert(space != NULL && *space != NULL);
	free(*space);
	*space = NULL;
}
