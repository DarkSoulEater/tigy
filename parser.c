//
// Created by Artyom Plevako on 25.02.2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

extern struct token gettok(FILE *);

static FILE *src;
static struct token cur;

static void expr();
static void func_call();
static void record_obj();
static void array_obj();
static void assign();
static void lvalue();
static void if_expr();
static void while_expr();
static void for_expr();
static void let_expr();
static void binary_op();
static void expr_seq();
static void expr_list();
static void field_list();
static void decl_list();
static void type_decl();
static void var_decl();
static void func_decl();
static void type_fields();

void parsefile(FILE *fp)
{
	src = fp;
	cur = gettok(src);
	expr();
	if (cur.name != EMPTY) {
		fputs("tigy: trailing code after program expression\n", stderr);
		exit(EXIT_FAILURE);
	}
}

void expr()
{
	switch (cur.name) {
	case ID:
		cur = gettok(src);
		if (!strcmp(cur.value, "(")) {
			func_call();
		} else if (!strcmp(cur.value, "{")) {
			record_obj();
		} else if (!strcmp(cur.value, "[")) {
			array_obj();
			if (strcmp(cur.value, "of")) {
				lvalue();
				assign();
			} else {
				cur = gettok(src);
				expr();
			}
		} else {
			lvalue();
			assign();
		}
		break;
	case KEYWORD:
		if (!strcmp(cur.value, "nil")) {
			cur = gettok(src);
		} else if (!strcmp(cur.value, "if")) {
			if_expr();
		} else if (!strcmp(cur.value, "while")) {
			while_expr();
		} else if (!strcmp(cur.value, "for")) {
			for_expr();
		} else if (!strcmp(cur.value, "break")) {
			cur = gettok(src);
		} else if (!strcmp(cur.value, "let")) {
			let_expr();
		} else {
			fputs("tigy: invalid keyword in expression\n", stderr);
			exit(EXIT_FAILURE);
		}
		break;
	case INT:
	case FLOAT:
	case STRING:
		cur = gettok(src);
		break;
	case PUNCTUATOR:
		if (!strcmp(cur.value, "-")) {
			cur = gettok(src);
			expr();
		} else if (!strcmp(cur.value, "(")) {
			cur = gettok(src);
			if (strcmp(cur.value, ")")) {
				expr_seq();
				if (strcmp(cur.value, ")")) {
					fputs("tigy: no closing bracket in "
                                               "expression sequence\n", stderr);
					exit(EXIT_FAILURE);
				}
			} else {
				cur = gettok(src);
			}
		} else {
			fputs("tigy: invalid punctuation in expression\n",
			      stderr);
			exit(EXIT_FAILURE);
		}
		break;
	case EMPTY:
		fputs("tigy: incomplete program\n", stderr);
		exit(EXIT_FAILURE);
	}
	binary_op();
}

void func_call()
{
	cur = gettok(src);
	if (strcmp(cur.value, ")")) {
		expr_list();
	}
	if (strcmp(cur.value, ")")) {
		fputs("tigy: no closing bracket in function call\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
}

void record_obj()
{
	cur = gettok(src);
	if (strcmp(cur.value, "}")) {
		field_list();
	}
	if (strcmp(cur.value, "}")) {
		fputs("tigy: no closing bracket in record object\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
}

void array_obj()
{
	cur = gettok(src);
	expr();
	if (strcmp(cur.value, "]")) {
		fputs("tigy: no closing bracket in array object\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
}

void assign()
{
	if (!strcmp(cur.value, ":=")) {
		cur = gettok(src);
		expr();
	}
}

void lvalue()
{
	if (!strcmp(cur.value, ".")) {
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no record member id in lvalue\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		lvalue();
	} else if (!strcmp(cur.value, "[")) {
		array_obj();
		lvalue();
	}
}

void if_expr()
{
	cur = gettok(src);
	expr();
	if (strcmp(cur.value, "then")) {
		fputs("tigy: no then in if\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
	if (!strcmp(cur.value, "else")) {
		cur = gettok(src);
		expr();
	}
}

void while_expr()
{
	cur = gettok(src);
	expr();
	if (strcmp(cur.value, "do")) {
		fputs("tigy: no do in while\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
}

void for_expr()
{
	cur = gettok(src);
	if (cur.name != ID) {
		fputs("tigy: no id in for\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, ":=")) {
		fputs("tigy: no assignment in for\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
	if (strcmp(cur.value, "to")) {
		fputs("tigy: no to in for\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
	if (strcmp(cur.value, "do")) {
		fputs("tigy: no do in for\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
}

void let_expr()
{
	cur = gettok(src);
	if (!strcmp(cur.value, "in")) {
		fputs("tigy: empty declaration list\n", stderr);
		exit(EXIT_FAILURE);
	}
	decl_list();
	if (strcmp(cur.value, "in")) {
		fputs("tigy: no \"in\" in let\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, "end")) {
		expr_seq();
	}
	if (strcmp(cur.value, "end")) {
		fputs("tigy: no \"end\" in let\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
}

void binary_op()
{
	if (!strcmp(cur.value, "&") || !strcmp(cur.value, "|") ||
	    !strcmp(cur.value, ">=") || !strcmp(cur.value, "<=") ||
	    !strcmp(cur.value, "<") || !strcmp(cur.value, ">") ||
	    !strcmp(cur.value, "=") || !strcmp(cur.value, "<>") ||
	    !strcmp(cur.value, "*") || !strcmp(cur.value, "/") ||
	    !strcmp(cur.value, "+") || !strcmp(cur.value, "-")) {
		cur = gettok(src);
		expr();
	}
}

void expr_seq()
{
	expr();
	while (!strcmp(cur.value, ";")) {
		cur = gettok(src);
		expr();
	}
}

void expr_list()
{
	expr();
	while (!strcmp(cur.value, ",")) {
		cur = gettok(src);
		expr();
	}
}

void field_list()
{
	if (cur.name != ID) {
		fputs("tigy: no id in field list\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, "=")) {
		fputs("tigy: no = in field list\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
	while (!strcmp(cur.value, ",")) {
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no id in field list\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		if (strcmp(cur.value, "=")) {
			fputs("tigy: no = in field list\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		expr();
	}
}

void decl_list()
{
	if (!strcmp(cur.value, "type")) {
		type_decl();
		decl_list();
	} else if (!strcmp(cur.value, "var")) {
		var_decl();
		decl_list();
	} else if (!strcmp(cur.value, "function")) {
		func_decl();
		decl_list();
	} else
		return;
}

void type_decl()
{
	cur = gettok(src);
	if (cur.name != ID) {
		fputs("tigy: no type-id in type declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, "=")) {
		fputs("tigy: no = in type declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (cur.name == ID) {
		cur = gettok(src);
	} else if (!strcmp(cur.value, "{")) {
		cur = gettok(src);
		if (strcmp(cur.value, "}")) {
			type_fields();
			if (strcmp(cur.value, "}")) {
				fputs("tigy: no closing bracket in type "
                                                       "declaration\n", stderr);
				exit(EXIT_FAILURE);
			}
		}
		cur = gettok(src);
	} else if (!strcmp(cur.value, "array")) {
		cur = gettok(src);
		if (strcmp(cur.value, "of")) {
			fputs("tigy: no of in type declaration\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no type-id in type declaration\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
	}
}

void var_decl()
{
	cur = gettok(src);
	if (cur.name != ID) {
		fputs("tigy: no id in var declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (!strcmp(cur.value, ":")) {
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no type-id in var declaration\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
	}
	if (strcmp(cur.value, ":=")) {
		fputs("tigy: no := in var declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
}

void func_decl()
{
	cur = gettok(src);
	if (cur.name != ID) {
		fputs("tigy: no id in function declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, "(")) {
		fputs("tigy: no ( in function declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, ")")) {
		type_fields();
		if (strcmp(cur.value, ")")) {
			fputs("tigy: no ) in function declaration\n", stderr);
			exit(EXIT_FAILURE);
		}
	}
	cur = gettok(src);
	if (!strcmp(cur.value, ":")) {
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no type-id in function declaration\n",
                              stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
	}
	if (strcmp(cur.value, "=")) {
		fputs("tigy: no = in function declaration\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	expr();
}

void type_fields()
{
	if (cur.name != ID) {
		fputs("tigy: no id in type-fields\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (strcmp(cur.value, ":")) {
		fputs("tigy: no : in type-fields\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	if (cur.name != ID) {
		fputs("tigy: no type-id in type-fields\n", stderr);
		exit(EXIT_FAILURE);
	}
	cur = gettok(src);
	while (!strcmp(cur.value, ",")) {
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no id in type-fields\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		if (strcmp(cur.value, ":")) {
			fputs("tigy: no : in type-fields\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
		if (cur.name != ID) {
			fputs("tigy: no type-id in type-fields\n", stderr);
			exit(EXIT_FAILURE);
		}
		cur = gettok(src);
	}
}
