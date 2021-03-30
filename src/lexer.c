#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "source-file.h"
#include "token.h"

static int get_character(struct source_file *file)
{
	int character = getc(file->data);
	if (character == '\n')
		file->column = 1, file->line++;
	else if (character != EOF)
		file->column++;
	return character;
}

static int undo_get_character(int character, struct source_file *file)
{
	int result = ungetc(character, file->data);
	if (result == '\n')
		file->line--;
	else if (result != EOF)
		file->column--;
	return result;
}

static void finish_word(struct source_file *file, char *word)
{
	const char *start = word++;
	int character;
	bool is_first_error = true;
	while (isalnum(character = get_character(file)) || character == '_') {
		if (word - start < IDENTIFIER_LENGTH - 1)
			*word++ = (char) character;
		else if (is_first_error) {
			is_first_error = false;
			print_error(file, file->line, file->column,
				"invalid identifier length, limit - %d", IDENTIFIER_LENGTH - 1);
		}
	}
	undo_get_character(character, file);
	*word = '\0';
}

static enum token_kind keyword_kind(const char *keyword)
{
	if (strcmp(keyword, "array") == 0)
		return ARRAY_KEYWORD;
	else if (strcmp(keyword, "break") == 0)
		return BREAK_KEYWORD;
	else if (strcmp(keyword, "do") == 0)
		return DO_KEYWORD;
	else if (strcmp(keyword, "else") == 0)
		return ELSE_KEYWORD;
	else if (strcmp(keyword, "end") == 0)
		return END_KEYWORD;
	else if (strcmp(keyword, "for") == 0)
		return FOR_KEYWORD;
	else if (strcmp(keyword, "function") == 0)
		return FUNCTION_KEYWORD;
	else if (strcmp(keyword, "if") == 0)
		return IF_KEYWORD;
	else if (strcmp(keyword, "in") == 0)
		return IN_KEYWORD;
	else if (strcmp(keyword, "let") == 0)
		return LET_KEYWORD;
	else if (strcmp(keyword, "nil") == 0)
		return NIL_KEYWORD;
	else if (strcmp(keyword, "of") == 0)
		return OF_KEYWORD;
	else if (strcmp(keyword, "then") == 0)
		return THEN_KEYWORD;
	else if (strcmp(keyword, "to") == 0)
		return TO_KEYWORD;
	else if (strcmp(keyword, "type") == 0)
		return TYPE_KEYWORD;
	else if (strcmp(keyword, "var") == 0)
		return VAR_KEYWORD;
	else if (strcmp(keyword, "while") == 0)
		return WHILE_KEYWORD;
	else
		return NONE;
}

static void finish_integer(struct source_file *file, int *integer)
{
	int character;
	while (isdigit(character = get_character(file)))
		*integer = (*integer * 10) + (character - '0');
	if (isalpha(character)) {
		print_error(file, file->line, file->column,
			"invalid character '%c' in the number constant", character);
		while (isalnum(character = get_character(file)))
			;
	}
	undo_get_character(character, file);
}

static void finish_float(struct source_file *file, float *floating)
{
	float base = 1;
	int character = get_character(file);
	if (!isdigit(character)) {
		print_error(file, file->line, file->column, "empty fractional part");
		undo_get_character(character, file);
		return;
	}
	while (isdigit(character = get_character(file)))
		*floating += (base /= 10) * (float) (character - '0');
	if (isalpha(character)) {
		print_error(file, file->line, file->column,
			"invalid character '%c' in the float constant", character);
		while (isalnum(character = get_character(file)))
			;
	}
	undo_get_character(character, file);
}

static void finish_string(struct source_file *file, char *string)
{
	const char *start = string;
	bool is_first_error = true;
	while (true) {
		int digit_character;
		switch (*string++ = (char) get_character(file)) {
		case EOF:
			print_error(file, file->line, file->column,
				"missing terminating \" in the string constant");
			return;
		case '"':
			*--string = '\0';
			return;
		case '\\':
			switch (*--string = (char) get_character(file)) {
			case 'n': *string = '\n'; break;
			case 't': *string = '\t'; break;
			case '"': *string = '"'; break;
			case '\\': break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				*string -= '0';
				if (isdigit(digit_character = get_character(file)))
					*string = (char) ((*string * 10) + (digit_character - '0'));
				else {
					undo_get_character(digit_character, file);
					print_error(file, file->line, file->column,
						"too few digits in the escape sequence");
					break;
				}
				if (isdigit(digit_character = get_character(file)))
					*string = (char) ((*string * 10) + (digit_character - '0'));
				else {
					undo_get_character(digit_character, file);
					print_error(file, file->line, file->column,
						"too few digits in the escape sequence");
					break;
				}
				break;
			case '\n': case ' ' : case '\t': case '\v': case '\f': case '\r':
				while (isspace(*string = (char) get_character(file)))
					;
				if (*string != '\\') {
					undo_get_character(*string, file);
					print_error(file, file->line, file->column,
						"missing terminating \\ in the whitespace escape sequence");
				}
				string--;
				break;
			default:
				print_error(file, file->line, file->column, "invalid escape sequence '%c'", *string);
				break;
			}
			string++;
			break;
		default: break;
		}
		if (string - start >= STRING_CONSTANT_LENGTH) {
			if (is_first_error) {
				is_first_error = false;
				print_error(file, file->line, file->column,
					"invalid string constant length, limit - %d",
					STRING_CONSTANT_LENGTH - 1);
			}
			string--;
		}
	}
}

static void consume_comment(struct source_file *file)
{
	while (true) {
		int character = get_character(file);
		switch (character) {
		case '*':
			if ((character = get_character(file)) == '/')
				return;
			else
				undo_get_character(character, file);
		case '/':
			if ((character = get_character(file)) == '*')
				consume_comment(file);
			else
				undo_get_character(character, file);
			break;
		case EOF:
			print_error(file, file->line, file->column, "missing terminating */ in the comment");
			return;
		default: break;
		}
	}
}

struct token get_token(struct source_file *file)
{
	assert(file != NULL);
	struct token token;
	int character;
	while (isspace(character = get_character(file)))
		;
	token.line = file->line, token.column = file->column - 1;
	switch (character) {
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k':
	case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v':
	case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K':
	case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
	case 'W': case 'X': case 'Y': case 'Z':
		*token.value.identifier = (char) character;
		finish_word(file, token.value.identifier);
		token.name = keyword_kind(token.value.identifier);
		if (token.name == NONE)
			token.name = IDENTIFIER;
		break;
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		token.value.integer_constant = character - '0';
		finish_integer(file, &token.value.integer_constant);
		if ((character = get_character(file)) == '.') {
			finish_float(file, &token.value.float_constant);
			token.name = FLOAT_CONSTANT;
		} else {
			undo_get_character(character, file);
			token.name = INTEGER_CONSTANT;
		}
		break;
	case '"':
		token.name = STRING_CONSTANT;
		finish_string(file, token.value.string_constant);
		break;
	case ',': token.name = COMMA; break;
	case ':':
		switch (character = get_character(file)) {
		case '=': token.name = ASSIGNMENT; break;
		default: token.name = COLON; undo_get_character(character, file); break;
		}
		break;
	case ';': token.name = SEMICOLON; break;
	case '(': token.name = LEFT_PARENTHESIS; break;
	case ')': token.name = RIGHT_PARENTHESIS; break;
	case '[': token.name = LEFT_BRACKET; break;
	case ']': token.name = RIGHT_BRACKET; break;
	case '{': token.name = LEFT_BRACE; break;
	case '}': token.name = RIGHT_BRACE; break;
	case '.': token.name = DOT; break;
	case '+': token.name = PLUS; break;
	case '-': token.name = MINUS; break;
	case '*': token.name = ASTERISK; break;
	case '/':
		switch (character = get_character(file)) {
		case '*':
			consume_comment(file);
			return get_token(file);
		default: token.name = SLASH; undo_get_character(character, file); break;
		}
		break;
	case '=': token.name = EQUAL; break;
	case '<':
		switch (character = get_character(file)) {
		case '>': token.name = NOT_EQUAL; break;
		case '=': token.name = LESS_EQUAL; break;
		default: token.name = LESS; undo_get_character(character, file); break;
		}
		break;
	case '>':
		switch (character = get_character(file)) {
		case '=': token.name = GREATER_EQUAL; break;
		default: token.name = GREATER; undo_get_character(character, file); break;
		}
		break;
	case '&': token.name = AND; break;
	case '|': token.name = OR; break;
	case EOF: token.name = NONE; break;
	default:
		print_error(file, file->line, file->column, "invalid character '%c'", character);
		return get_token(file);
	}
	return token;
}
