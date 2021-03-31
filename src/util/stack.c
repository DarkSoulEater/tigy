//
// Created by Artyom Plevako on 29.03.2021.
//

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util/stack.h"

struct stack {
	size_t size;
	struct node {
		void *value;
		struct node *link;
	} *data;
};

struct stack *stack_allocate(void)
{
	struct stack *stack = malloc(sizeof(struct stack));
	stack->size = 0;
	stack->data = NULL;
	return stack;
}

bool stack_is_empty(struct stack *stack)
{
	assert(stack != NULL);
	return stack->size == 0;
}

void stack_push(struct stack *stack, void *value)
{
	assert(stack != NULL);
	struct node *data = malloc(sizeof(struct node));
	data->value = value;
	data->link = stack->data;
	stack->data = data;
	stack->size++;
}

void *stack_pop(struct stack *stack)
{
	assert(stack != NULL && stack->data != NULL);
	struct node *data = stack->data;
	stack->data = data->link;
	stack->size--;
	void *value = data->value;
	free(data);
	return value;
}

void stack_free(struct stack **stack)
{
	assert(stack != NULL && *stack != NULL);
	while (!stack_is_empty(*stack))
		stack_pop(*stack);
	free(*stack);
	*stack = NULL;
}
