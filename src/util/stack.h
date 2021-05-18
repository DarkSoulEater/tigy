/**
 * \file
 * \brief
 */

#ifndef TIGY_STACK_H
#define TIGY_STACK_H

#include <stdbool.h>

/**
 *
 * @return
 */
extern struct stack *stack_allocate(void);

/**
 *
 * @param stack
 * @return
 */
extern bool stack_is_empty(struct stack *stack);

/**
 *
 * @param stack
 * @return
 */
extern void *stack_peak(struct stack *stack);

/**
 *
 * @param stack
 * @param value
 */
extern void stack_push(struct stack *stack, void *value);

/**
 *
 * @param stack
 * @return
 */
extern void *stack_pop(struct stack *stack);

/**
 *
 * @param stack
 */
extern void stack_free(struct stack **stack);

#endif //TIGY_STACK_H
