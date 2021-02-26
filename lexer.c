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

struct token gettok(FILE *fp) {
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
                        token.name = STRING;
                        i = -1;
                        while ((cur[++i] = getc(fp)) != '"') {
                                if (i >= TOKENLEN) {
                                        printf("Error: to many long str const\n");
                                        exit(EXIT_FAILURE);
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
                                                        printf("Error: expected escape sequences\n");
                                                        exit(EXIT_FAILURE);
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
                                if (i >= TOKENLEN) {
                                        printf("Error: to many long int const\n");
                                        exit(EXIT_FAILURE);
                                }
                        }
                        if (cur[i] == '.') {
                                token.name = FLOAT;
                                while (isdigit(cur[++i] = getc(fp))) {
                                        if (i >= TOKENLEN) {
                                                printf("Error: to many long int const\n");
                                                exit(EXIT_FAILURE);
                                        }
                                }
                        }
                        if (cur[i] != EOF) ungetc(cur[i], fp);
                        cur[i] = '\0';
                        if (cur[i - 1] == '.') {
                                printf("Error: fractional part missing\n");
                                exit(EXIT_FAILURE);
                        }
                        return token;
                }

                default: {
                        while (isalpha(cur[i]) || cur[i] == '_' || isdigit(cur[i])) {
                                cur[++i] = getc(fp);
                                if (i >= TOKENLEN) {
                                        printf("Error: to many long id\n");
                                        exit(EXIT_FAILURE);
                                }
                        }
                        if (cur[i] != EOF) ungetc(cur[i], fp);
                        if (i == 0) {
                                printf("Error: unknow character %c\n", cur[i]);
                                exit(EXIT_FAILURE);
                        }
                        cur[i] = '\0';
                        token.name = (is_keyword(token.value) ? KEYWORD : ID);
                        return token;
                }
        }
}
