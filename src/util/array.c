//
// Created by Artyom Plevako on 29.03.2021.
//

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "array.h"

struct array {
	size_t size, capacity;
	void **data;
};

struct array *array_allocate(void)
{
	struct array *array = malloc(sizeof(struct array));
	array->size = array->capacity = 0;
	array->data = NULL;
	return array;
}

void array_push_back(struct array *array, void *value)
{
	assert(array != NULL);
	if (array->size == array->capacity) {
		array->capacity = array->capacity == 0 ? 1 : 2 * array->capacity;
		array->data = realloc(array->data, sizeof(void *) * array->capacity);
	}
	array->data[array->size++] = value;
}

void *array_pop_back(struct array *array)
{
	assert(array != NULL && array->size != 0);
	return array->data[--array->size];
}

void *array_at_index(struct array *array, size_t index)
{
	assert(array != NULL && index < array->size);
	return array->data[index];
}

size_t array_size(struct array *array)
{
	assert(array != NULL);
	return array->size;
}

bool array_is_empty(struct array *array)
{
	assert(array != NULL);
	return array->size == 0;
}

void array_free(struct array **array)
{
	assert(array != NULL && *array != NULL);
	free((*array)->data);
	free(*array);
	*array = NULL;
}
