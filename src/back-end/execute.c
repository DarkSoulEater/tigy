//
// Created by Artyom Plevako on 31.03.2021.
//

#include "back-end/execute.h"
#include "front-end/semantics/scope.h"
#include "util/array.h"
#include "util/stack.h"

struct stack *execution_buffer;

void execute_scope(struct scope *scope)
{
	for (int i = 0; i < array_size(scope->execution_sequence); i++) {
		struct execution_unit *unit = (struct execution_unit *) array_at_index(scope->execution_sequence, i);
		if (unit->kind == OPERAND)
			stack_push(execution_buffer, unit->data);
		else
			((void (*)(void)) unit->data)();
	}
}

void execute_program(struct program *program)
{
	execution_buffer = stack_allocate();
	execute_scope(program_current_scope(program));
	stack_free(&execution_buffer);
}
