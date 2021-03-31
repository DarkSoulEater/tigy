//
// Created by Artyom Plevako on 30.03.2021.
//

#include <assert.h>
#include <stdlib.h>

#include "front-end/semantics/program.h"
#include "front-end/semantics/scope.h"
#include "front-end/semantics/types.h"

struct program {
	struct scope *data;
};

struct program *program_allocate(void)
{
	struct program *program = malloc(sizeof(struct program));
	program->data = scope_allocate();

	struct type *int_type = malloc(sizeof(struct type));
	int_type->kind = INT;
	int_type->data = NULL;
	scope_insert_type(program->data, "int", int_type);

	struct type *float_type = malloc(sizeof(struct type));
	float_type->kind = FLOAT;
	float_type->data = NULL;
	scope_insert_type(program->data, "float", float_type);

	struct type *string_type = malloc(sizeof(struct type));
	string_type->kind = STRING;
	string_type->data = NULL;
	scope_insert_type(program->data, "string", string_type);

	return program;
}

void program_free(struct program **program)
{
	assert(program != NULL && *program != NULL);
	free(*program);
	*program = NULL;
}

void namespace_begin_scope(struct program *program)
{
	assert(program != NULL);
	struct scope *scope = scope_allocate();
	scope->parent = program->data;
	program->data = scope;
}

void program_end_scope(struct program *program)
{
	assert(program != NULL);
	program->data = program->data->parent;
}

struct scope *program_current_scope(struct program *program)
{
	assert(program != NULL);
	return program->data;
}
