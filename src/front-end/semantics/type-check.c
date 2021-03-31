//
// Created by Artyom Plevako on 31.03.2021.
//

#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "type-check.h"
#include "../syntax/token.h"

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

static struct type* get_type(struct source_file *file, struct token *operation)
{
        if (stack_is_empty(expression_types))
                print_error(file, operation->line, operation->column,"not found token");
        return (struct type *)stack_pop(expression_types);
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
                print_error(file, operation->line, operation->column,"not algebraic operators");
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
                print_error(file, operation->line, operation->column,"not equality operators");
}

void check_comparison_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (lhs->kind != STRING || rhs->kind != STRING))
                print_error(file, operation->line, operation->column,"not comparison operators");
}

void check_logical_operation(struct source_file *file, struct token *operation)
{
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if (!is_algebraic(rhs) || !is_algebraic(lhs))
                print_error(file, operation->line, operation->column,"not algebraic operators");
}

void check_assigmend_operation(struct source_file *file, struct token *operation) {
        assert(file != NULL && operation != NULL);
        struct type *rhs = get_type(file, operation);
        struct type *lhs = get_type(file, operation);
        if ((!is_algebraic(rhs) || !is_algebraic(lhs))
            && (rhs->kind != STRING || lhs->kind != STRING)
            && (rhs->kind != ARRAY || lhs->kind != ARRAY || rhs->data != lhs->data)
            && (rhs->kind != RECORD || lhs->kind != RECORD || rhs->data != lhs->data))
                print_error(file, operation->line, operation->column,"not assigmend operators");
}
