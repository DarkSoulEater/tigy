#include <stdio.h>
#include "token.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int is_keyword(const char *c) {
        return !(strcmp(c, "array") && strcmp(c, "break") && strcmp(c, "do") && strcmp(c, "else") && strcmp(c, "end") &&
                 strcmp(c, "for") && strcmp(c, "function") && strcmp(c, "if") && strcmp(c, "in") && strcmp(c, "let") &&
                 strcmp(c, "nil") && strcmp(c, "of") && strcmp(c, "then") && strcmp(c, "to") && strcmp(c, "type") &&
                 strcmp(c, "var") && strcmp(c, "while"));
}

struct token get_token(FILE *fp) {
        struct token token;
        char *cur = token.value;

        label_skip:
        while (isspace(*cur = getc(fp)))
                ;
        if (*cur == EOF) {
                token.name = EMPTY;
                *cur = '\0';
                return token;
        }
        if (*cur == '/') {
                char cur = getc(fp), prev;
                int comment_cnt = 1;
                if(cur == '*') {
                        cur = '/';
                        while (comment_cnt) {
                                prev = cur;
                                cur = getc(fp);
                                if (cur == EOF) {
                                        printf("Error: expected end of comment\n");
                                        exit(EXIT_FAILURE);
                                }
                                if (cur == '*' && prev == '/') ++comment_cnt;
                                else if (cur == '/' && prev == '*') --comment_cnt;
                        }
                        goto label_skip;
                } else {
                        ungetc(cur, fp);
                }
        }

        switch (*cur) {
                case ':': case '<': case '>': {
                        token.name = PUNCTUATOR;
                        if ((*++cur = getc(fp)) == '=') {
                                *++cur = '\0';
                        } else {
                                if (*cur != EOF) ungetc(*cur, fp);
                                *cur = '\0';
                        }
                        return token;
                }

                case ',': case ';': case '(': case ')':
                case '[': case ']': case '{': case '}':
                case '.': case '+': case '-': case '*':
                case '/': case '=': case '&': case '|': {
                        token.name = PUNCTUATOR;
                        *++cur = '\0';
                        return token;
                }

                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': {
                        token.name = INT;
                        while (isdigit(*++cur = getc(fp)));
                        if (*cur == '.') {
                                token.name = FLOAT;
                                while (isdigit(*++cur = getc(fp)));
                        }
                        if (*cur != EOF) ungetc(*cur, fp);
                        *cur = '\0';
                        // if (*(c - 1) == '.') trow error;
                        return token;
                }

                default: {
                        while (isalpha(*cur) || *cur == '_' || isdigit(*cur)) {
                                *++cur = getc(fp); // TODO: add limit to size;
                        }
                        if (*cur != EOF) ungetc(*cur, fp);
                        *cur = '\0';
                        token.name = (token.value[0] == '\0' ? EMPTY : (is_keyword(token.value) ? KEYWORD : ID));
                        //token.name = (is_keyword(token.value) ? KEYWORD : ID);
                        return token;
                }
        }
}
