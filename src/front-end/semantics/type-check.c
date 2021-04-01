//
// Created by Artyom Plevako on 31.03.2021.
//

#include <assert.h>
#include <stdlib.h>

#include "types.h"
#include "type-check.h"
#include "../syntax/token.h"

struct stack *expression_types;

struct type *string_type;
struct type *int_type;
struct type *float_type;

void init_type_check(void)
{
        expression_types = stack_allocate();

        string_type = malloc(sizeof(struct type));
        int_type = malloc(sizeof(struct type));
        float_type = malloc(sizeof(struct type));

        string_type->kind = STRING;
	int_type->kind = INT;
	float_type->kind = FLOAT;

	string_type->data = NULL;
	int_type->data = NULL;
	float_type->data = NULL;
}

void clean_up_type_check(void)
{
        stack_free(&expression_types);

        free(string_type);
        free(int_type);
        free(float_type);
}

static struct type* get_type(void)
{
	assert(!stack_is_empty(expression_types));
        return stack_pop(expression_types);
}

static bool is_algebraic(struct type* type) {
        return type->kind == INT || type->kind == FLOAT;
}

void check_algebraic_operation(struct source_file *file, struct token operation)
{
        assert(file != NULL);
        struct type *rhs = get_type();
        struct type *lhs = get_type();
        if (!is_algebraic(rhs) || !is_algebraic(lhs))
                print_error(file, operation.line, operation.column,"invalid algebraic operator operands type");
        else {
        	if (lhs->kind == FLOAT)
        		rhs->kind = FLOAT;
        	stack_push(expression_types, rhs);
        }
}

void check_comparison_operation(struct source_file *file, struct token operation)
{
        assert(file != NULL);
        struct type *rhs = get_type();
        struct type *lhs = get_type();
        if (operation.name == EQUAL || operation.name == NOT_EQUAL) {
		if ((!is_algebraic(rhs) || !is_algebraic(lhs))
			&& (rhs->kind != STRING || lhs->kind != STRING)
			&& (rhs->kind != ARRAY || lhs->kind != ARRAY || rhs->data != lhs->data)
			&& (rhs->kind != RECORD || lhs->kind != RECORD || rhs->data != lhs->data))
			print_error(file, operation.line, operation.column,"invalid equality operator operands type");
		else {
			rhs->kind = INT;
			stack_push(expression_types, rhs);
		}
        } else if ((!is_algebraic(rhs) || !is_algebraic(lhs))
        	&& (lhs->kind != STRING || rhs->kind != STRING)) {
                print_error(file, operation.line, operation.column,"invalid comparison operator operands type");
        } else {
		rhs->kind = INT;
		stack_push(expression_types, rhs);
	}
}

void check_logical_operation(struct source_file *file, struct token operation)
{
        assert(file != NULL);
        struct type *rhs = get_type();
        struct type *lhs = get_type();
        if (!is_algebraic(rhs) || !is_algebraic(lhs))
                print_error(file, operation.line, operation.column,"invalid logical operator operands type");
        else {
        	rhs->kind = INT;
		stack_push(expression_types, rhs);
	}
}

void check_assignment_operation(struct source_file *file, struct token operation) {
        assert(file != NULL);
        struct type *rhs = get_type();
        struct type *lhs = get_type();
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (rhs->kind != STRING || lhs->kind != STRING)
            && (rhs->kind != ARRAY || lhs->kind != ARRAY || rhs->data != lhs->data)
            && (rhs->kind != RECORD || lhs->kind != RECORD || rhs->data != lhs->data))
                print_error(file, operation.line, operation.column,"invalid assignment operator operands type");
        else
        	stack_push(expression_types, NULL);
}

void check_unary_minus(struct source_file *file, struct token operation)
{
	assert(file != NULL);
	struct type *rhs = get_type();
	if (!is_algebraic(rhs))
		print_error(file, operation.line, operation.column,"invalid unary minus operand type");
	else
		stack_push(expression_types, rhs);
}
