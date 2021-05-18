/**
 * \file
 * \brief
 */
#ifndef TIGY_ARRAY_H
#define TIGY_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

/**
 *
 * @return
 */
extern struct array *array_allocate(void);

/**
 *
 * @param array
 * @param value
 */
extern void array_push_back(struct array *array, void *value);

/**
 *
 * @param array
 * @return
 */
extern void *array_pop_back(struct array *array);

/**
 *
 * @param array
 * @param index
 * @return
 */
extern void *array_at_index(struct array *array, size_t index);

/**
 *
 * @param array
 * @return
 */
extern size_t array_size(struct array *array);

/**
 *
 * @param array
 * @return
 */
extern bool array_is_empty(struct array *array);

/**
 *
 * @param array
 */
extern void array_free(struct array **array);

#endif //TIGY_ARRAY_H
