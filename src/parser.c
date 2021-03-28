//
// Created by Artyom Plevako on 25.02.2021.
//

#include <assert.h>

#include "source-file.h"
#include "token.h"

static struct source_file *source_file;
static struct token current_token;

static const char *token_type_string[] = {
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
	"'nil'",
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

static void parse_next_token(enum token_type type)
{
	if (current_token.name != type)
		print_error(source_file, current_token.line, current_token.column,
			"expected %s", token_type_string[type]);
	else
		current_token = get_token(source_file);
}

static void parse_sequence(
	enum token_type end,
	void (*element)(void),
	enum token_type delimiter)
{
	if (current_token.name == end)
		current_token = get_token(source_file);
	else {
		while (element(), current_token.name == delimiter)
			current_token = get_token(source_file);
		parse_next_token(end);
	}
}

static void parse_type_field(void)
{
	parse_next_token(IDENTIFIER);
	parse_next_token(COLON);
	parse_next_token(IDENTIFIER);
}

static void parse_type_definition(void)
{
	switch (current_token.name) {
	case LEFT_BRACE:
		current_token = get_token(source_file);
		parse_sequence(RIGHT_BRACE, parse_type_field, COMMA);
		break;
	case ARRAY_KEYWORD:
		current_token = get_token(source_file);
		parse_next_token(OF_KEYWORD);
		parse_next_token(IDENTIFIER);
		break;
	case IDENTIFIER:
		current_token = get_token(source_file);
		break;
	default:
		print_error(source_file, current_token.line, current_token.column, "expected type definition");
		break;
	}
}

static void parse_type_specifier(void)
{
	if (current_token.name == COLON) {
		current_token = get_token(source_file);
		parse_next_token(IDENTIFIER);
	}
}

static void parse_expression(void);

static void parse_function_declaration(void)
{
	parse_next_token(FUNCTION_KEYWORD);
	parse_next_token(IDENTIFIER);
	parse_next_token(LEFT_PARENTHESIS);
	parse_sequence(RIGHT_PARENTHESIS, parse_type_field, COMMA);
	parse_type_specifier();
	parse_next_token(EQUAL);
	parse_expression();
}

static void parse_variable_declaration(void)
{
	parse_next_token(VAR_KEYWORD);
	parse_next_token(IDENTIFIER);
	parse_type_specifier();
	parse_next_token(ASSIGNMENT);
	parse_expression();
}

static void parse_type_declaration(void)
{
	parse_next_token(TYPE_KEYWORD);
	parse_next_token(IDENTIFIER);
	parse_next_token(EQUAL);
	parse_type_definition();
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

static void parse_record_field(void)
{
	parse_next_token(IDENTIFIER);
	parse_next_token(EQUAL);
	parse_expression();
}

static void parse_lvalue_suffix(void)
{
	switch (current_token.name) {
	case DOT:
		current_token = get_token(source_file);
		parse_next_token(IDENTIFIER);
		break;
	case LEFT_BRACKET:
		current_token = get_token(source_file);
		parse_expression();
		parse_next_token(RIGHT_BRACKET);
		break;
	default:
		return;
	}
	parse_lvalue_suffix();
}

static void parse_if_expression(void)
{
	parse_expression();
	parse_next_token(THEN_KEYWORD);
	parse_expression();
	if (current_token.name == ELSE_KEYWORD) {
		current_token = get_token(source_file);
		parse_expression();
	}
}

static void parse_while_expression(void)
{
	parse_expression();
	parse_next_token(DO_KEYWORD);
	parse_expression();
}

static void parse_for_expression(void)
{
	parse_next_token(IDENTIFIER);
	parse_next_token(ASSIGNMENT);
	parse_expression();
	parse_next_token(TO_KEYWORD);
	parse_expression();
	parse_next_token(DO_KEYWORD);
	parse_expression();
}

static void parse_let_expression(void)
{
	if (current_token.name == IN_KEYWORD) {
		print_error(source_file, current_token.line, current_token.column, "empty declaration list");
		current_token = get_token(source_file);
	} else {
		parse_declaration_list();
		parse_next_token(IN_KEYWORD);
	}
	parse_sequence(END_KEYWORD, parse_expression, SEMICOLON);
}

static void parse_identifier(void)
{
	switch (current_token.name) {
	case LEFT_BRACE:
		current_token = get_token(source_file);
		parse_sequence(RIGHT_BRACE, parse_record_field, COMMA);
		break;
	case LEFT_BRACKET:
		current_token = get_token(source_file);
		parse_expression();
		parse_next_token(RIGHT_BRACKET);
		if (current_token.name == OF_KEYWORD) {
			current_token = get_token(source_file);
			parse_expression();
		} else {
	default:
			parse_lvalue_suffix();
			if (current_token.name == ASSIGNMENT) {
				current_token = get_token(source_file);
				parse_expression();
			}
		}
		break;
	case LEFT_PARENTHESIS:
		current_token = get_token(source_file);
		parse_sequence(RIGHT_PARENTHESIS, parse_expression, COMMA);
		break;
	}
}

static void parse_primary_expression(void)
{
	switch (current_token.name) {
	case STRING_CONSTANT:
	case INTEGER_CONSTANT:
	case FLOAT_CONSTANT:
	case NIL_KEYWORD:
	case BREAK_KEYWORD:
		current_token = get_token(source_file);
		break;
	case MINUS:
		current_token = get_token(source_file);
		parse_expression();
		break;
	case LEFT_PARENTHESIS:
		current_token = get_token(source_file);
		parse_sequence(RIGHT_PARENTHESIS, parse_expression, SEMICOLON);
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
	case IDENTIFIER:
		current_token = get_token(source_file);
		parse_identifier();
		break;
	default:
		print_error(source_file, current_token.line, current_token.column, "expected expression");
		break;
	}
}

static void parse_multiplicative_expression(void)
{
	while (parse_primary_expression(), current_token.name == ASTERISK || current_token.name == SLASH)
		current_token = get_token(source_file);
}

static void parse_additive_expression(void)
{
	while (parse_multiplicative_expression(), current_token.name == PLUS || current_token.name == MINUS)
		current_token = get_token(source_file);
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
	while (parse_additive_expression(), is_comparison_operator())
		current_token = get_token(source_file);
}

static void parse_and_expression(void)
{
	while (parse_comparison_expression(), current_token.name == AND)
		current_token = get_token(source_file);
}

static void parse_or_expression(void)
{
	while (parse_and_expression(), current_token.name == OR)
		current_token = get_token(source_file);
}

static void parse_expression(void)
{
	parse_or_expression();
}

void parse_source_file(struct source_file *file)
{
	assert(file != NULL);
	source_file = file;
	current_token = get_token(source_file);
	parse_expression();
	if (current_token.name != NONE)
		print_error(source_file, current_token.line, current_token.column,
			"trailing code after the main expression");
}
