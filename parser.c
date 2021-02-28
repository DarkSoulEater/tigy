//
// Created by Artyom Plevako on 25.02.2021.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diagnostics.h"
#include "token.h"

extern struct token gettoken(FILE *);

static FILE *src;
static struct token cur;

static void expr       (void);
static void func_call  (void);
static void record_obj (void);
static void array_obj  (void);
static void assign     (void);
static void lvalue     (void);
static void if_expr    (void);
static void while_expr (void);
static void for_expr   (void);
static void let_expr   (void);
static void binary_expr(void);
static void expr_seq   (void);
static void expr_list  (void);
static void field_list (void);
static void decl_list  (void);
static void type_decl  (void);
static void var_decl   (void);
static void func_decl  (void);
static void type_fields(void);
static void id         (void);
static void typeid     (void);
static void next       (const char *);

void parsefile(FILE *fp)
{
	src = fp;
	cur = gettoken(src);
	expr();
	if (cur.name != EMPTY)
		error("expected", "end of file", cur.line, cur.col);
}

void expr()
{
	switch (cur.name) {
	case ID:
		cur = gettoken(src);
		if (strcmp(cur.value, "(") == 0)
			func_call();
		else if (strcmp(cur.value, "{") == 0)
			record_obj();
		else if (strcmp(cur.value, "[") == 0) {
			array_obj();
			if (strcmp(cur.value, "of") != 0) {
				lvalue();
				assign();
			} else {
				cur = gettoken(src);
				expr();
			}
		} else {
			lvalue();
			assign();
		}
		break;
	case KEYWORD:
		if (strcmp(cur.value, "nil") == 0)
			cur = gettoken(src);
		else if (strcmp(cur.value, "if") == 0)
			if_expr();
		else if (strcmp(cur.value, "while") == 0)
			while_expr();
		else if (strcmp(cur.value, "for") == 0)
			for_expr();
		else if (strcmp(cur.value, "break") == 0)
			cur = gettoken(src);
		else if (strcmp(cur.value, "let") == 0)
			let_expr();
		else
			error("expected", "expression", cur.line, cur.col);
		break;
	case INT:
	case FLOAT:
	case STR:
		cur = gettoken(src);
		break;
	case PUNCTUATOR:
		if (strcmp(cur.value, "-") == 0) {
			cur = gettoken(src);
			expr();
		} else if (strcmp(cur.value, "(") == 0) {
			cur = gettoken(src);
			if (strcmp(cur.value, ")") != 0) {
				expr_seq();
				next(")");
			} else {
				cur = gettoken(src);
			}
		} else
			error("expected", "expression", cur.line, cur.col);
		break;
	case EMPTY:
		error("expected", "expression", cur.line, cur.col);
		break;
	}
	binary_expr();
}

void func_call()
{
	cur = gettoken(src);
	if (strcmp(cur.value, ")") != 0)
		expr_list();
	next(")");
}

void record_obj()
{
	cur = gettoken(src);
	if (strcmp(cur.value, "}") != 0)
		field_list();
	next("}");
}

void array_obj()
{
	cur = gettoken(src);
	expr();
	next("]");
}

void assign()
{
	if (strcmp(cur.value, ":=") == 0) {
		cur = gettoken(src);
		expr();
	}
}

void lvalue()
{
	if (strcmp(cur.value, ".") == 0) {
		cur = gettoken(src);
		lvalue();
	} else if (strcmp(cur.value, "[") == 0) {
		array_obj();
		lvalue();
	}
}

void if_expr()
{
	cur = gettoken(src);
	expr();
	next("then");
	expr();
	if (strcmp(cur.value, "else") == 0) {
		cur = gettoken(src);
		expr();
	}
}

void while_expr()
{
	cur = gettoken(src);
	expr();
	next("do");
	expr();
}

void for_expr()
{
	cur = gettoken(src);
	id();
	next(":=");
	expr();
	next("to");
	expr();
	next("do");
	expr();
}

void let_expr()
{
	cur = gettoken(src);
	if (strcmp(cur.value, "in") == 0)
		error("expected", "declaration list", cur.line, cur.col);
	decl_list();
	next("in");
	if (strcmp(cur.value, "end") != 0) {
		expr_seq();
	}
	next("end");
}

void binary_expr()
{
	if (strcmp(cur.value, "&") == 0 || strcmp(cur.value, "|") == 0 ||
	    strcmp(cur.value, ">=") == 0 || strcmp(cur.value, "<=") == 0 ||
	    strcmp(cur.value, "<") == 0 || strcmp(cur.value, ">") == 0 ||
	    strcmp(cur.value, "=") == 0 || strcmp(cur.value, "<>") == 0 ||
	    strcmp(cur.value, "*") == 0 || strcmp(cur.value, "/") == 0 ||
	    strcmp(cur.value, "+") == 0 || strcmp(cur.value, "-") == 0) {
		cur = gettoken(src);
		expr();
	}
}

void expr_seq()
{
	expr();
	while (strcmp(cur.value, ";") == 0) {
		cur = gettoken(src);
		expr();
	}
}

void expr_list()
{
	expr();
	while (strcmp(cur.value, ",") == 0) {
		cur = gettoken(src);
		expr();
	}
}

void field_list()
{
	id();
	next("=");
	expr();
	while (strcmp(cur.value, ",") == 0) {
		cur = gettoken(src);
		id();
		next("=");
		expr();
	}
}

void decl_list()
{
	if (strcmp(cur.value, "type") == 0) {
		type_decl();
		decl_list();
	} else if (strcmp(cur.value, "var") == 0) {
		var_decl();
		decl_list();
	} else if (strcmp(cur.value, "function") == 0) {
		func_decl();
		decl_list();
	} else
		return;
}

void type_decl()
{
	cur = gettoken(src);
	typeid();
	next("=");
	if (strcmp(cur.value, "{") == 0) {
		cur = gettoken(src);
		if (strcmp(cur.value, "}") != 0) {
			type_fields();
			next("}");
		} else
			cur = gettoken(src);
	} else if (strcmp(cur.value, "array") == 0) {
		cur = gettoken(src);
		next("of");
		typeid();
	} else
		typeid();
}

void var_decl()
{
	cur = gettoken(src);
	id();
	if (strcmp(cur.value, ":") == 0) {
		cur = gettoken(src);
		typeid();
	}
	next(":=");
	expr();
}

void func_decl()
{
	cur = gettoken(src);
	id();
	next("(");
	if (strcmp(cur.value, ")") != 0) {
		type_fields();
		next(")");
	} else
		cur = gettoken(src);
	if (strcmp(cur.value, ":") == 0) {
		cur = gettoken(src);
		typeid();
	}
	next("=");
	expr();
}

void type_fields()
{
	id();
	next(":");
	typeid();
	while (strcmp(cur.value, ",") == 0) {
		cur = gettoken(src);
		id();
		next(":");
		typeid();
	}
}

void next(const char *lexeme)
{
	if (strcmp(cur.value, lexeme) != 0)
		error("expected", lexeme, cur.line, cur.col);
	cur = gettoken(src);
}

void id(void)
{
	if (cur.name != ID)
		error("expected", "identifier", cur.line, cur.col);
	cur = gettoken(src);
}

void typeid(void)
{
	if (cur.name != ID)
		error("expected", "type-identifier", cur.line, cur.col);
	cur = gettoken(src);
}
