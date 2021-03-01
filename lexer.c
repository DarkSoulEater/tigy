#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "diagnostics.h"
#include "token.h"

static int iskeyword(const char *);

struct token gettoken(FILE *fp)
{
	struct token token;
	char* cur = token.value;
	int i = 0;

	label_skip:
	while (isspace(cur[i] = getc(fp)))
		;
	if (cur[i] == EOF) {
		token.name = EMPTY;
		cur[i] = '\0';
		return token;
	}
	if (cur[i] == '/') {
		char cur = getc(fp), prev;
		int comment_cnt = 1;
		if(cur == '*') {
			cur = '.';
			while (comment_cnt) {
				prev = cur;
				cur = getc(fp);
				if (cur == EOF) {
					error("expected", "end of comment", 0, 0);
				}
				if (cur == '*' && prev == '/') ++comment_cnt;
				else if (cur == '/' && prev == '*') --comment_cnt;
			}
			goto label_skip;
		} else {
			ungetc(cur, fp);
		}
	}

	switch (cur[i]) {
	case ':': case '<': case '>': {
		token.name = PUNCTUATOR;
		if ((cur[++i] = getc(fp)) == '=') {
			cur[++i] = '\0';
		} else {
			if (cur[i] != EOF) ungetc(cur[i], fp);
			cur[i] = '\0';
		}
		return token;
	}

	case ',': case ';': case '(': case ')':
	case '[': case ']': case '{': case '}':
	case '.': case '+': case '-': case '*':
	case '/': case '=': case '&': case '|': {
		token.name = PUNCTUATOR;
		cur[++i] = '\0';
		return token;
	}

	case '"': {
		token.name = STR;
		i = -1;
		while ((cur[++i] = getc(fp)) != '"') {
			if (i >= TOKLEN) {
				error("invalid", "string length", 0, 0);
			}
			if (cur[i] == '\\') {
				char tmp = getc(fp);
				switch (tmp) {
				case 'n': {
					cur[i] = '\n';
					break;
				}

				case 't': {
					cur[i] = '\t';
					break;
				}

				case '"': {
					cur[i] = '"';
					break;
				}

				case '\\': {
					cur[i] = '\\';
					break;
				}

				default: {
					error("invalid", "escape sequence", 0, 0);
				}
				}
			}
		}
		cur[i] = '\0';
		return token;
	}

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': {
		token.name = INT;
		while (isdigit(cur[++i] = getc(fp))) {
			if (i >= TOKLEN) {
				error("invalid", "integer length", 0, 0);
			}
		}
		if (cur[i] == '.') {
			token.name = FLOAT;
			while (isdigit(cur[++i] = getc(fp))) {
				if (i >= TOKLEN) {
					error("invalid", "float length", 0, 0);
				}
			}
		}
		if (cur[i] != EOF) ungetc(cur[i], fp);
		cur[i] = '\0';
		if (cur[i - 1] == '.') {
			error("expected", "fractional part", 0, 0);
		}
		return token;
	}

	default: {
		while (isalpha(cur[i]) || cur[i] == '_' || isdigit(cur[i])) {
			cur[++i] = getc(fp);
			if (i >= TOKLEN) {
				error("invalid", "identifier length", 0, 0);
			}
		}
		if (cur[i] != EOF) ungetc(cur[i], fp);
		if (i == 0) {
			error("invalid", "character", 0, 0);
		}
		cur[i] = '\0';
		token.name = (iskeyword(token.value) ? KEYWORD : ID);
		return token;
	}
	}
}

int iskeyword(const char *s)
{
	return !(strcmp(s, "array") && strcmp(s, "break") && strcmp(s, "do") &&
		strcmp(s, "else") && strcmp(s, "end") && strcmp(s, "for") &&
		strcmp(s, "function") && strcmp(s, "if") && strcmp(s, "in") &&
		strcmp(s, "let") && strcmp(s, "nil") && strcmp(s, "of") &&
		strcmp(s, "then") && strcmp(s, "to") && strcmp(s, "type") &&
		strcmp(s, "var") && strcmp(s, "while"));
}
