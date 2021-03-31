//
// Created by Artyom Plevako on 31.03.2021.
//

#include <stdlib.h>

#include "types.h"
#include "type-check.h"

struct namespace *type_namespace, *variable_namespace, *function_namespace;
struct stack *expression_types;

void init_namespaces(void)
{
	type_namespace = namespace_allocate();
	variable_namespace = namespace_allocate();
	function_namespace = namespace_allocate();

	namespace_begin_scope(type_namespace);
	namespace_begin_scope(variable_namespace);
	namespace_begin_scope(function_namespace);

	struct type *int_type = malloc(sizeof(struct type));
	int_type->kind = INT;
	int_type->data = NULL;
	namespace_insert(type_namespace, "int", int_type);

	struct type *float_type = malloc(sizeof(struct type));
	float_type->kind = FLOAT;
	float_type->data = NULL;
	namespace_insert(type_namespace, "float", float_type);

	struct type *string_type = malloc(sizeof(struct type));
	string_type->kind = STRING;
	string_type->data = NULL;
	namespace_insert(type_namespace, "string", string_type);
}

void clean_up_namespaces(void)
{
	namespace_free(&type_namespace);
	namespace_free(&variable_namespace);
	namespace_free(&function_namespace);
}

void init_type_check(void)
{
	expression_types = stack_allocate();
}

void clean_up_type_check(void)
{
	stack_free(&expression_types);
}
