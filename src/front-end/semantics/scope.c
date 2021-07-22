#include <assert.h>
#include <stdlib.h>

#include "front-end/semantics/scope.h"
#include "util/array.h"
#include "util/hash-table.h"

struct scope *scope_allocate(void)
{
	struct scope *scope = malloc(sizeof(struct scope));

	scope->type_namespace = table_allocate();
	scope->variable_namespace = table_allocate();
	scope->function_namespace = table_allocate();

	scope->execution_sequence = array_allocate();

	scope->parent = NULL;

	return scope;
}

void scope_free(struct scope **scope)
{
	assert(scope != NULL && *scope != NULL);

	table_free(&(*scope)->type_namespace);
	table_free(&(*scope)->variable_namespace);
	table_free(&(*scope)->function_namespace);

	array_free(&(*scope)->execution_sequence);

	free(*scope);
	*scope = NULL;
}

void scope_insert_type(struct scope *scope, const char *key, void *value)
{
	assert(scope != NULL && key != NULL && value != NULL);
	table_insert(scope->type_namespace, key, value);
}

void *scope_lookup_type(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	return table_lookup(scope->type_namespace, key);
}

void *scope_deep_lookup_type(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	for (; scope != NULL; scope = scope->parent)
		if (table_lookup(scope->type_namespace, key) != NULL)
			return table_lookup(scope->type_namespace, key);
	return NULL;
}

void scope_insert_variable(struct scope *scope, const char *key, void *value)
{
	assert(scope != NULL && key != NULL && value != NULL);
	table_insert(scope->variable_namespace, key, value);
}

void *scope_lookup_variable(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	return table_lookup(scope->variable_namespace, key);
}

void *scope_deep_lookup_variable(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	for (; scope != NULL; scope = scope->parent)
		if (table_lookup(scope->variable_namespace, key) != NULL)
			return table_lookup(scope->variable_namespace, key);
	return NULL;
}

void scope_insert_function(struct scope *scope, const char *key, void *value)
{
	assert(scope != NULL && key != NULL && value != NULL);
	table_insert(scope->function_namespace, key, value);
}

void *scope_lookup_function(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	return table_lookup(scope->function_namespace, key);
}

void *scope_deep_lookup_function(struct scope *scope, const char *key)
{
	assert(scope != NULL && key != NULL);
	for (; scope != NULL; scope = scope->parent)
		if (table_lookup(scope->function_namespace, key) != NULL)
			return table_lookup(scope->function_namespace, key);
	return NULL;
}
