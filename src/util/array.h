//
// Created by Artyom Plevako on 29.03.2021.
//

#ifndef TIGY_ARRAY_H
#define TIGY_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

extern struct array *array_allocate(void);
extern void array_push_back(struct array *array, void *value);
extern void *array_pop_back(struct array *array);
extern void *array_at_index(struct array *array, size_t index);
extern size_t array_size(struct array *array);
extern bool array_is_empty(struct array *array);
extern void array_free(struct array **array);

#endif //TIGY_ARRAY_H
