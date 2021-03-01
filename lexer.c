#include <ctype.h>
#include <stdio.h>
#include "diagnostics.h"
#include "token.h"
#include "hashtable.h"

static int line = 0;
static int col = 0;

static int get(FILE* fp);
static void unget(int ch, FILE *fp);
static int iskeyword(const char *);

struct token gettoken(FILE *fp)
{
	struct token token;
	char* cur = token.value;
	int i = 0;

	label_skip:
	while (isspace(cur[i] = get(fp)))
		;
	if (cur[i] == EOF) {
		token.name = EMPTY;
		cur[i] = '\0';
		return token;
	}
	if (cur[i] == '/') {
		char cur = get(fp), prev;
		int comment_cnt = 1;
		if(cur == '*') {
			cur = '.';
			while (comment_cnt) {
				prev = cur;
				cur = get(fp);
				if (cur == EOF) {
					error("expected", "end of comment", line, col);
				}
				if (cur == '*' && prev == '/') ++comment_cnt;
				else if (cur == '/' && prev == '*') --comment_cnt;
			}
			goto label_skip;
		} else {
			unget(cur, fp);
		}
	}

	token.line = line, token.col = col;
	switch (cur[i]) {
	case ':': case '<': case '>': {
		token.name = PUNCTUATOR;
		if ((cur[++i] = get(fp)) == '=') {
			cur[++i] = '\0';
		} else {
			if (cur[i] != EOF) unget(cur[i], fp);
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
		while ((cur[++i] = get(fp)) != '"') {
			if (i >= TOKLEN) {
				error("invalid", "string length", line, col);
			}
			if (cur[i] == '\\') {
				char tmp = get(fp);
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
					error("invalid", "escape sequence", line, col);
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
		while (isdigit(cur[++i] = get(fp))) {
			if (i >= TOKLEN) {
				error("invalid", "integer length", line, col);
			}
		}
		if (cur[i] == '.') {
			token.name = FLOAT;
			while (isdigit(cur[++i] = get(fp))) {
				if (i >= TOKLEN) {
					error("invalid", "float length", line, col);
				}
			}
		}
		if (cur[i] != EOF) unget(cur[i], fp);
		cur[i] = '\0';
		if (cur[i - 1] == '.') {
			error("expected", "fractional part", line, col);
		}
		return token;
	}

	default: {
		while (isalpha(cur[i]) || cur[i] == '_' || isdigit(cur[i])) {
			cur[++i] = get(fp);
			if (i >= TOKLEN) {
				error("invalid", "identifier length", line, col);
			}
		}
		if (cur[i] != EOF) unget(cur[i], fp);
		if (i == 0) {
			error("invalid", "character", line, col);
		}
		cur[i] = '\0';
		token.name = (iskeyword(token.value) ? KEYWORD : ID);
		return token;
	}
	}
}

int get(FILE* fp)
{
        int c = getc(fp);
        (c == '\n' ? ++line, col = 0 : ++col);
        return c;
}

void unget(int ch, FILE *fp) {
        ungetc(ch, fp);
        --col;
}

int iskeyword(const char *s)
{
        static struct hashtable tbkey;
        if (tbkey.size == 0) {
                table_create(&tbkey);
                table_insert(&tbkey, "array");
                table_insert(&tbkey, "break");
                table_insert(&tbkey, "do");
                table_insert(&tbkey, "else");
                table_insert(&tbkey, "end");
                table_insert(&tbkey, "for");
                table_insert(&tbkey, "function");
                table_insert(&tbkey, "if");
                table_insert(&tbkey, "in");
                table_insert(&tbkey, "let");
                table_insert(&tbkey, "nil");
                table_insert(&tbkey, "of");
                table_insert(&tbkey, "then");
                table_insert(&tbkey, "to");
                table_insert(&tbkey, "type");
                table_insert(&tbkey, "var");
                table_insert(&tbkey, "while");
        }
	return table_find(&tbkey, s);
}
