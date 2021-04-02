//
// Created by Artyom Plevako on 25.02.2021.
//

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "front-end/syntax/source-file.h"
#include "front-end/syntax/token.h"
#include "front-end/semantics/types.h"
#include "front-end/semantics/type-check.h"
#include "front-end/semantics/program.h"
#include "front-end/semantics/scope.h"

static struct source_file *source_file;
static struct token current_token;
static struct program *program;

static const char *token_kind_string[] = {
	"",
	"identifier",
	"integer constant",
	"string constant",
	"float constant",
	"'array'",
	"'break'",
	"'do'",
	"'else'",
	"'end'",
	"'for'",
	"'function'",
	"'if'",
	"'in'",
	"'let'",
	"'of'",
	"'then'",
	"'to'",
	"'type'",
	"'var'",
	"'while'",
	",",
	":",
	";",
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	".",
	"+",
	"-",
	"*",
	"/",
	"=",
	"<>",
	"<",
	"<=",
	">",
	">=",
	"&",
	"|",
	":=",
};

static void parse_next_token(enum token_kind kind)
{
	if (current_token.name != kind)
		print_error(source_file, current_token.line, current_token.column,
			"expected %s", token_kind_string[kind]);
	else
		current_token = get_token(source_file);
}

static void parse_expression(void);

static void parse_expression_sequence(enum token_kind end)
{
	if (current_token.name == end) {
		if (source_file->is_correct)
			stack_push(expression_types, NULL);
		current_token = get_token(source_file);
	} else {
		while (parse_expression(), current_token.name == SEMICOLON) {
			if (source_file->is_correct)
				stack_pop(expression_types);
			current_token = get_token(source_file);
		}
		parse_next_token(end);
	}
}

static void parse_expression_list(enum token_kind end, struct token function_identifier)
{
	struct function *function;
	if (source_file->is_correct) {
		function = scope_deep_lookup_function(program_current_scope(program), function_identifier.value.identifier);
		if (function == NULL)
			print_error(source_file, function_identifier.line, function_identifier.column,
				"use of undeclared function identifier '%s'", function_identifier.value.identifier);
	}
	if (current_token.name == end) {
		if (source_file->is_correct && !array_is_empty(function->fields))
			print_error(source_file, function_identifier.line, function_identifier.column,
				"too few arguments in function call, expected %d, have 0", array_size(function->fields));
		current_token = get_token(source_file);
	} else {
		int i;
		for (i = 0; parse_expression(), current_token.name == COMMA; i++) {
			if (source_file->is_correct && i < array_size(function->fields) && ((struct field *) array_at_index(function->fields, i))->type != stack_pop(expression_types))
				print_error(source_file, function_identifier.line, function_identifier.column,
					"function '%s' arguments and parameters does not match", function_identifier.value.identifier);
			current_token = get_token(source_file);
		}
		if (source_file->is_correct && i < array_size(function->fields) && ((struct field *) array_at_index(function->fields, i))->type != stack_pop(expression_types))
			print_error(source_file, function_identifier.line, function_identifier.column,
				"function '%s' arguments and parameters does not match", function_identifier.value.identifier);
		if (source_file->is_correct && array_size(function->fields) != i + 1)
			print_error(source_file, function_identifier.line, function_identifier.column,
				"function '%s' arguments and parameters have different length", function_identifier.value.identifier);
		parse_next_token(end);
	}
	if (source_file->is_correct)
		stack_push(expression_types, function->return_type);
}

static bool has_record_field(struct record *record, struct token identifier, struct type *type)
{
	for (int i = 0; i < array_size(record->fields); i++)
		if (strcmp(((struct field *) array_at_index(record->fields, i))->identifier, identifier.value.identifier) == 0)
			return ((struct field *) array_at_index(record->fields, i))->type == type;
	return false;
}

static void parse_field(struct record *record)
{
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	parse_next_token(EQUAL);
	parse_expression();
	struct type *type;
	if (source_file->is_correct && !has_record_field(record, identifier, type = stack_pop(expression_types)))
		print_error(source_file, identifier.line, identifier.column,
			"no matching actual field in record initialization for '%s'", identifier.value.identifier);
}

static void parse_field_list(enum token_kind end, struct token record_identifier)
{
	struct record *record;
	if (source_file->is_correct) {
		struct type *type = scope_deep_lookup_type(program_current_scope(program), record_identifier.value.identifier);
		if (type == NULL || type->kind != RECORD)
			print_error(source_file, record_identifier.line, record_identifier.column,
				"use of undeclared record type '%s'", record_identifier.value.identifier);
		else
			record = type->data;
	}
	if (current_token.name == end) {
		if (source_file->is_correct && !array_is_empty(record->fields))
			print_error(source_file, record_identifier.line, record_identifier.column,
				"too few fields in record initialization, expected %d, have 0", array_size(record->fields));
		current_token = get_token(source_file);
	} else {
		int i;
		for (i = 0; parse_field(record), current_token.name == COMMA; i++)
			current_token = get_token(source_file);
		if (source_file->is_correct && array_size(record->fields) != i + 1)
			print_error(source_file, record_identifier.line, record_identifier.column,
				"record '%s' initialization fields and actual fields have different length", record_identifier.value.identifier);
		parse_next_token(end);
	}
	if (source_file->is_correct)
		stack_push(expression_types, scope_deep_lookup_type(program_current_scope(program), record_identifier.value.identifier));
}

static struct type *find_field_type(struct record *record, const char *field)
{
	for (int i = 0; i < array_size(record->fields); i++)
		if (strcmp(((struct field *) array_at_index(record->fields, i))->identifier, field) == 0)
			return ((struct field *) array_at_index(record->fields, i))->type;
	return NULL;
}

static void parse_type_field(struct record *record)
{
	struct field *field = malloc(sizeof(struct field));
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	if (source_file->is_correct) {
		field->identifier = malloc(sizeof(char[IDENTIFIER_LENGTH]));
		strcpy(field->identifier, identifier.value.identifier);
		if (find_field_type(record, field->identifier) != NULL)
			print_error(source_file, identifier.line, identifier.column,
				"record has fields with the same name '%s'", identifier.value.identifier);
	}
	parse_next_token(COLON);
	identifier = current_token;
	parse_next_token(IDENTIFIER);
	if (source_file->is_correct) {
		field->type = scope_deep_lookup_type(program_current_scope(program), identifier.value.identifier);
		if (field->type == NULL)
			print_error(source_file, identifier.line, identifier.column,
				"use of undeclared field type '%s'", identifier.value.identifier);
		array_push_back(record->fields, field);
	}
}

static void parse_record_type(enum token_kind end, struct token type_identifier)
{
	struct type *record_type;
	struct record *record;
	if (source_file->is_correct) {
		record_type = malloc(sizeof(struct type));
		record_type->kind = RECORD;
		record = malloc(sizeof(struct record));
		record_type->data = record;
		record->fields = array_allocate();
	}
	if (current_token.name == end)
		current_token = get_token(source_file);
	else {
		while (parse_type_field(record), current_token.name == COMMA)
			current_token = get_token(source_file);
		parse_next_token(end);
	}
	if (source_file->is_correct)
		scope_insert_type(program_current_scope(program), type_identifier.value.identifier, record_type);
}

static void parse_array_type(struct token type_identifier)
{
	parse_next_token(OF_KEYWORD);
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	if (source_file->is_correct) {
		struct type *array_type = malloc(sizeof(struct type));
		array_type->kind = ARRAY;
		array_type->data = scope_deep_lookup_type(program_current_scope(program), identifier.value.identifier);
		if (array_type->data == NULL)
			print_error(source_file, identifier.line, identifier.column,
				"use of undeclared type '%s'", identifier.value.identifier);
		else
			scope_insert_type(program_current_scope(program), type_identifier.value.identifier, array_type);
	}
}

static void parse_synonymous_type(struct token type_identifier, struct token synonymous_type)
{
	if (source_file->is_correct) {
		struct type *type = malloc(sizeof(struct type));
		struct type *synonym = scope_deep_lookup_type(program_current_scope(program), synonymous_type.value.identifier);
		if (synonym == NULL)
			print_error(source_file, synonymous_type.line, synonymous_type.column,
				"use of undeclared type '%s'", synonymous_type.value.identifier);
		else {
			type->kind = synonym->kind;
			type->data = synonym->data;
			scope_insert_type(program_current_scope(program), type_identifier.value.identifier, type);
		}
	}
}

static void parse_type_definition(struct token type_identifier)
{
	if (source_file->is_correct && scope_lookup_type(program_current_scope(program), type_identifier.value.identifier) != NULL)
			print_error(source_file, type_identifier.line, type_identifier.column,
				"redefinition of the type '%s'", type_identifier.value.identifier);
	switch (current_token.name) {
	case LEFT_BRACE:
		current_token = get_token(source_file);
		parse_record_type(RIGHT_BRACE, type_identifier);
		break;
	case ARRAY_KEYWORD:
		current_token = get_token(source_file);
		parse_array_type(type_identifier);
		break;
	case IDENTIFIER:
		parse_synonymous_type(type_identifier, current_token);
		current_token = get_token(source_file);
		break;
	default:
		print_error(source_file, current_token.line, current_token.column, "expected type definition");
		break;
	}
}

static void parse_function_parameter(struct array *fields)
{
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	parse_next_token(COLON);
	struct token type_identifier = current_token;
	parse_next_token(IDENTIFIER);
	if (source_file->is_correct) {
		struct field *field = malloc(sizeof(struct field));
		field->identifier = identifier.value.identifier;
		field->type = scope_deep_lookup_type(program_current_scope(program), type_identifier.value.identifier);
		struct variable *variable = malloc(sizeof(struct variable));
		variable->type = field->type;
		variable->data = NULL;
		if (source_file->is_correct && scope_lookup_variable(program_current_scope(program), identifier.value.identifier) != NULL)
			print_error(source_file, identifier.line, identifier.column,
				"function has parameters with the same name '%s'", identifier.value.identifier);
		if (source_file->is_correct)
			scope_insert_variable(program_current_scope(program), identifier.value.identifier, variable);
		if (field == NULL)
			print_error(source_file, type_identifier.line, type_identifier.column,
				"use of undeclared type '%s'", type_identifier.value.identifier);
		array_push_back(fields, field);
	}
}

static void parse_function_parameters(struct function *function)
{
	if (current_token.name == RIGHT_PARENTHESIS)
		current_token = get_token(source_file);
	else {
		while (parse_function_parameter(function->fields), current_token.name == COMMA)
			current_token = get_token(source_file);
		parse_next_token(RIGHT_PARENTHESIS);
	}
}

static void parse_function_declaration(void)
{
	parse_next_token(FUNCTION_KEYWORD);
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	struct function *function;
	if (source_file->is_correct && scope_lookup_function(program_current_scope(program), identifier.value.identifier) != NULL)
		print_error(source_file, identifier.line, identifier.column,
			"redefinition of the function '%s'", identifier.value.identifier);
	parse_next_token(LEFT_PARENTHESIS);
	function = malloc(sizeof(struct function));
	function->fields = array_allocate();
	if (source_file->is_correct) {
		scope_insert_function(program_current_scope(program), identifier.value.identifier, function);
		function->scope = program_current_scope(program);
		program_begin_scope(program);
	}
	parse_function_parameters(function);
	if (current_token.name == COLON) {
		current_token = get_token(source_file);
		struct token type_identifier = current_token;
		parse_next_token(IDENTIFIER);
		if (source_file->is_correct) {
			function->return_type = scope_deep_lookup_type(program_current_scope(program), type_identifier.value.identifier);
			if (function->return_type == NULL)
				print_error(source_file, type_identifier.line, type_identifier.column,
					"use of undeclared type '%s'", type_identifier.value.identifier);
		}
	} else if (source_file->is_correct)
		function->return_type = NULL;
	parse_next_token(EQUAL);
	parse_expression();
	if (source_file->is_correct)
		program_end_scope(program);
}

static void parse_variable_declaration(void)
{
	parse_next_token(VAR_KEYWORD);
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	struct variable *variable = malloc(sizeof(struct variable));
	if (source_file->is_correct && scope_lookup_variable(program_current_scope(program), identifier.value.identifier) != NULL)
		print_error(source_file, identifier.line, identifier.column,
			"redefinition of the variable '%s'", identifier.value.identifier);
	if (current_token.name == COLON) {
		current_token = get_token(source_file);
		struct token type_identifier = current_token;
		parse_next_token(IDENTIFIER);
		parse_next_token(ASSIGNMENT);
		parse_expression();
		struct type *type;
		if (source_file->is_correct && (type = scope_deep_lookup_type(program_current_scope(program), type_identifier.value.identifier)) == NULL)
			print_error(source_file, type_identifier.line, type_identifier.column,
				"use of undeclared type '%s'", type_identifier.value.identifier);
		if (source_file->is_correct && stack_peak(expression_types) != type)
			print_error(source_file, type_identifier.line, type_identifier.column,
				"expression and specified type '%s' does not match", type_identifier.value.identifier);
	} else {
		parse_next_token(ASSIGNMENT);
		parse_expression();
	}
	if (source_file->is_correct) {
		variable->type = stack_pop(expression_types);
		scope_insert_variable(program_current_scope(program), identifier.value.identifier, variable);
	}
}

static void parse_type_declaration(void)
{
	parse_next_token(TYPE_KEYWORD);
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	parse_next_token(EQUAL);
	parse_type_definition(identifier);
}

static void parse_declaration_list(void)
{
	switch (current_token.name) {
	case TYPE_KEYWORD: parse_type_declaration(); break;
	case VAR_KEYWORD: parse_variable_declaration(); break;
	case FUNCTION_KEYWORD: parse_function_declaration(); break;
	default: return;
	}
	parse_declaration_list();
}

static void parse_if_expression(void)
{
	parse_expression();
	struct type *type;
	if (source_file->is_correct) {
		type = stack_pop(expression_types);
		if (type == NULL || type->kind != INT)
			print_error(source_file, current_token.line, current_token.column,
				"condition expression must be an integer");
	}
	parse_next_token(THEN_KEYWORD);
	parse_expression();
	if (source_file->is_correct)
		type = stack_pop(expression_types);
	if (current_token.name == ELSE_KEYWORD) {
		current_token = get_token(source_file);
		parse_expression();
		if (source_file->is_correct) {
			struct type *second_type = stack_pop(expression_types);
			if (type != second_type)
				print_error(source_file, current_token.line, current_token.column,
					"'if' expressions must be of the same type");
			else
				stack_push(expression_types, type);
		}
	} else if (source_file->is_correct)
		stack_push(expression_types, NULL);
}

static void parse_while_expression(void)
{
	parse_expression();
	struct type *type;
	if (source_file->is_correct) {
		type = stack_pop(expression_types);
		if (type == NULL && type->kind != INT)
			print_error(source_file, current_token.line, current_token.column,
				"'while' expression must be an integer");
	}
	parse_next_token(DO_KEYWORD);
	parse_expression();
}

static void parse_for_expression(void)
{
	struct token identifier = current_token;
	parse_next_token(IDENTIFIER);
	struct variable *variable;
	if (source_file->is_correct) {
		variable = malloc(sizeof(struct variable));
		variable->type = int_type;
		scope_insert_variable(program_current_scope(program), identifier.value.identifier, variable);
	}
	parse_next_token(ASSIGNMENT);
	parse_expression();
	struct type *type;
	if (source_file->is_correct) {
		type = stack_pop(expression_types);
		if (type == NULL && type->kind != INT)
			print_error(source_file, current_token.line, current_token.column,
				"'for' expression must be an integer");
	}
	parse_next_token(TO_KEYWORD);
	parse_expression();
	if (source_file->is_correct) {
		type = stack_pop(expression_types);
		if (type == NULL && type->kind != INT)
			print_error(source_file, current_token.line, current_token.column,
				"'for' expression must be an integer");
	}
	parse_next_token(DO_KEYWORD);
	parse_expression();
}

static void parse_let_expression(void)
{
	if (source_file->is_correct)
		program_begin_scope(program);
	if (current_token.name == IN_KEYWORD) {
		print_error(source_file, current_token.line, current_token.column, "empty declaration list");
		current_token = get_token(source_file);
	} else {
		parse_declaration_list();
		parse_next_token(IN_KEYWORD);
	}
	parse_expression_sequence(END_KEYWORD);
	if (source_file->is_correct)
		program_end_scope(program);
}

static void parse_lvalue_suffix(struct token identifier)
{
	struct type *type;
	switch (current_token.name) {
	case DOT:
		if (source_file->is_correct) {
			type = stack_pop(expression_types);
			if (type->kind != RECORD)
				print_error(source_file, identifier.line, identifier.column,
					"'%s' is not a record", identifier.value.identifier);
		}
		current_token = get_token(source_file);
		struct token field = current_token;
		parse_next_token(IDENTIFIER);
		if (source_file->is_correct) {
			type = find_field_type(type->data, field.value.identifier);
			if (type == NULL)
				print_error(source_file, field.line, field.column,
					"use of unknown field '%s'", field.value.identifier);
			else
				stack_push(expression_types, type);
		}
		break;
	case LEFT_BRACKET:
		if (source_file->is_correct) {
			type = stack_pop(expression_types);
			if (type->kind != ARRAY)
				print_error(source_file, identifier.line, identifier.column,
					"'%s' is not an array");
		}
		current_token = get_token(source_file);
		parse_expression();
		parse_next_token(RIGHT_BRACKET);
		if (source_file->is_correct)
			stack_push(expression_types, type->data);
		break;
	default:
		return;
	}
	parse_lvalue_suffix(identifier);
}

static void parse_identifier(struct token identifier)
{
	struct variable *variable = scope_deep_lookup_variable(program_current_scope(program), identifier.value.identifier);
	switch (current_token.name) {
	case LEFT_BRACE:
		current_token = get_token(source_file);
		parse_field_list(RIGHT_BRACE, identifier);
		break;
	case LEFT_BRACKET:
		current_token = get_token(source_file);
		parse_expression();
		parse_next_token(RIGHT_BRACKET);
		if (current_token.name == OF_KEYWORD) {
			current_token = get_token(source_file);
			parse_expression();
			struct type *type;
			if (source_file->is_correct &&
				((type = scope_deep_lookup_type(program_current_scope(program), identifier.value.identifier)) == NULL ||
				type->kind != ARRAY))
				print_error(source_file, identifier.line, identifier.column,
					"use of undeclared array type '%s'", identifier.value.identifier);
			if (source_file->is_correct)
				stack_push(expression_types, type);
		} else {
			if (source_file->is_correct) {
				if (variable == NULL)
					print_error(source_file, identifier.line, identifier.column,
						"use of undeclared variable '%s'", identifier.value.identifier);
				else if (variable->type->kind != ARRAY)
					print_error(source_file, identifier.line, identifier.column,
						"'%s' is not an array");
				else
					stack_push(expression_types, variable->type->data);
			}
			parse_lvalue_suffix(identifier);
			struct token operation = current_token;
			if (current_token.name == ASSIGNMENT) {
				current_token = get_token(source_file);
				parse_expression();
				if (source_file->is_correct)
					check_assignment_operation(source_file, operation);
			}
		}
		break;
	default: if (source_file->is_correct) {
			if (variable == NULL)
				print_error(source_file, identifier.line, identifier.column,
					"use of undeclared variable '%s'", identifier.value.identifier);
			else
				stack_push(expression_types, variable->type);
		}
		parse_lvalue_suffix(identifier);
		struct token operation = current_token;
		if (current_token.name == ASSIGNMENT) {
			current_token = get_token(source_file);
			parse_expression();
			if (source_file->is_correct)
				check_assignment_operation(source_file, operation);
		}
		break;
	case LEFT_PARENTHESIS:
		current_token = get_token(source_file);
		parse_expression_list(RIGHT_PARENTHESIS, identifier);
		break;
	}
}

static void parse_primary_expression(void)
{
	switch (current_token.name) {
	case STRING_CONSTANT: {
		stack_push(expression_types, string_type);
		current_token = get_token(source_file);
		break;
	} case INTEGER_CONSTANT: {
		stack_push(expression_types, int_type);
		current_token = get_token(source_file);
		break;
	} case FLOAT_CONSTANT: {
		stack_push(expression_types, float_type);
		current_token = get_token(source_file);
		break;
	} case BREAK_KEYWORD:
		stack_push(expression_types, NULL);
		current_token = get_token(source_file);
		break;
	case MINUS:
		current_token = get_token(source_file);
		parse_expression();
		if (source_file->is_correct)
			check_unary_minus(source_file, current_token);
		break;
	case LEFT_PARENTHESIS:
		current_token = get_token(source_file);
		parse_expression_sequence(RIGHT_PARENTHESIS);
		break;
	case IF_KEYWORD:
		current_token = get_token(source_file);
		parse_if_expression();
		break;
	case WHILE_KEYWORD:
		current_token = get_token(source_file);
		parse_while_expression();
		break;
	case FOR_KEYWORD:
		current_token = get_token(source_file);
		parse_for_expression();
		break;
	case LET_KEYWORD:
		current_token = get_token(source_file);
		parse_let_expression();
		break;
	case IDENTIFIER: {
		struct token identifier = current_token;
		current_token = get_token(source_file);
		parse_identifier(identifier);
		break;
	} default:
		print_error(source_file, current_token.line, current_token.column, "expected expression");
		break;
	}
}

static void parse_multiplicative_expression(void)
{
	parse_primary_expression();
	while (current_token.name == ASTERISK || current_token.name == SLASH) {
		struct token operation = current_token;
		current_token = get_token(source_file);
		parse_primary_expression();
		if (source_file->is_correct)
			check_algebraic_operation(source_file, operation);
	}
}

static void parse_additive_expression(void)
{
	parse_multiplicative_expression();
	while (current_token.name == PLUS || current_token.name == MINUS) {
		struct token operation = current_token;
		current_token = get_token(source_file);
		parse_multiplicative_expression();
		if (source_file->is_correct)
			check_algebraic_operation(source_file, operation);
	}
}

static bool is_comparison_operator(void)
{
	return current_token.name == EQUAL ||
		current_token.name == NOT_EQUAL ||
		current_token.name == LESS ||
		current_token.name == LESS_EQUAL ||
		current_token.name == GREATER ||
		current_token.name == GREATER_EQUAL;
}

static void parse_comparison_expression(void)
{
	parse_additive_expression();
	while (is_comparison_operator()) {
		struct token operation = current_token;
		current_token = get_token(source_file);
		parse_additive_expression();
		if (source_file->is_correct)
			check_comparison_operation(source_file, operation);
	}
}

static void parse_and_expression(void)
{
	parse_comparison_expression();
	while (current_token.name == AND) {
		struct token operation = current_token;
		current_token = get_token(source_file);
		parse_comparison_expression();
		if (source_file->is_correct)
			check_logical_operation(source_file, operation);
	}
}

static void parse_or_expression(void)
{
	parse_and_expression();
	while (current_token.name == OR) {
		struct token operation = current_token;
		current_token = get_token(source_file);
		parse_and_expression();
		if (source_file->is_correct)
			check_logical_operation(source_file, operation);
	}
}

static void parse_expression(void)
{
	parse_or_expression();
}

struct program *parse_source_file(struct source_file *file)
{
	assert(file != NULL);
	source_file = file;
	init_type_check();
	program = program_allocate();
	current_token = get_token(source_file);
	parse_expression();
	if (current_token.name != NONE)
		print_error(source_file, current_token.line, current_token.column,
			"trailing code after main expression");
	clean_up_type_check();
	return program;
}
