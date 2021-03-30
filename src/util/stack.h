//
// Created by Artyom Plevako on 29.03.2021.
//

#ifndef TIGY_STACK_H
#define TIGY_STACK_H

#include <stdbool.h>

extern struct stack *stack_allocate(void);
extern bool stack_is_empty(struct stack *stack);
extern void stack_push(struct stack *stack, void *value);
extern void *stack_pop(struct stack *stack);
extern void stack_free(struct stack **stack);

#endif //TIGY_STACK_H
