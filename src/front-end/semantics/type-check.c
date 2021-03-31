//
// Created by Artyom Plevako on 31.03.2021.
//

#include <assert.h>
#include <stdlib.h>

#include "types.h"
#include "type-check.h"
#include "../syntax/token.h"

struct stack *expression_types;

void init_type_check(void)
{
        expression_types = stack_allocate();
}

void clean_up_type_check(void)
{
        stack_free(&expression_types);
}

static struct type* get_type(struct source_file *file, struct token *operation)
{
        if (stack_is_empty(expression_types))
                print_error(file, operation->line, operation->column,"token not found");
        return (struct type *) stack_pop(expression_types);
}

static int is_algebraic(struct type* type) {
        return type->kind == INT || type->kind == FLOAT;
}

void check_algebraic_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if (!is_algebraic(rhs) || !is_algebraic(lhs))
                print_error(file, operation->line, operation->column,"invalid algebraic operator operands type");
}

void check_equality_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (rhs->kind != STRING || lhs->kind != STRING)
            && (rhs->kind != ARRAY || lhs->kind != ARRAY || rhs->data != lhs->data)
            && (rhs->kind != RECORD || lhs->kind != RECORD || rhs->data != lhs->data))
                print_error(file, operation->line, operation->column,"invalid equality operator operands type");
}

void check_comparison_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (lhs->kind != STRING || rhs->kind != STRING))
                print_error(file, operation->line, operation->column,"invalid comparison operator operands type");
}

void check_logical_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if (!is_algebraic(rhs) || !is_algebraic(lhs))
                print_error(file, operation->line, operation->column,"invalid logical operator operands type");
}

void check_assignment_operation(struct source_file *file, struct token *operation) {
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (rhs->kind != STRING || lhs->kind != STRING)
            && (rhs->kind != ARRAY || lhs->kind != ARRAY || rhs->data != lhs->data)
            && (rhs->kind != RECORD || lhs->kind != RECORD || rhs->data != lhs->data))
                print_error(file, operation->line, operation->column,"invalid assignment operator operands type");
}
